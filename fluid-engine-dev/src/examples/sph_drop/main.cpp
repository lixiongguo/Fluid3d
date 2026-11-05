#include "../example_io.h"

#include <jet/jet.h>

#include <cstdlib>
#include <iostream>
#include <string>

using namespace jet;

namespace {

void printUsage() {
    std::cerr << "Usage: sph_drop <output-dir> [frames]\n"
              << "  Writes one OBJ of particle positions per frame.\n"
              << "  frames defaults to 40, at 1/60 s.\n";
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 2 && argc != 3) {
        printUsage();
        return 1;
    }

    const std::string outputDir(argv[1]);
    int frameCount = 40;
    if (argc == 3) {
        frameCount = std::atoi(argv[2]);
        if (frameCount < 1) {
            std::cerr << "frames must be >= 1\n";
            return 1;
        }
    }
    if (!ensureDirectory(outputDir)) {
        return 1;
    }

    const double spacing = 0.04;
    const BoundingBox3D domain(Vector3D(), Vector3D(1.0, 1.0, 1.0));
    auto solver = SphSolver3::builder()
                      .withTargetDensity(kWaterDensity)
                      .withTargetSpacing(spacing)
                      .makeShared();
    solver->setViscosityCoefficient(0.01);

    auto container = Box3::builder()
                         .withIsNormalFlipped(true)
                         .withBoundingBox(domain)
                         .makeShared();
    solver->setCollider(
        RigidBodyCollider3::builder().withSurface(container).makeShared());

    auto drop = Sphere3::builder()
                    .withCenter({0.5, 0.65, 0.5})
                    .withRadius(0.15)
                    .makeShared();
    solver->setEmitter(VolumeParticleEmitter3::builder()
                           .withSurface(drop)
                           .withSpacing(spacing)
                           .withIsOneShot(true)
                           .makeShared());

    std::cout << "SPH drop, spacing " << spacing << ", " << frameCount
              << " frames\n";

    for (Frame frame(0, 1.0 / 60.0); frame.index < frameCount; ++frame) {
        solver->update(frame);
        const auto positions = solver->particleSystemData()->positions();
        const std::string path = framePath(outputDir, frame.index, "obj");
        if (!saveParticlesObj(path, positions)) {
            return 1;
        }
        std::cout << "frame " << frame.index << "  particles " << positions.size()
                  << "  " << path << "\n";
    }
    return 0;
}
