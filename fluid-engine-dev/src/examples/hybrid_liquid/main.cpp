#include "../example_io.h"

#include <jet/jet.h>

#include <cstdlib>
#include <iostream>
#include <string>

using namespace jet;

namespace {

PicSolver3Ptr makeSolver(const std::string& method, const Size3& resolution) {
    if (method == "pic") {
        return PicSolver3::builder()
            .withResolution(resolution)
            .withDomainSizeX(1.0)
            .makeShared();
    }
    if (method == "apic") {
        return ApicSolver3::builder()
            .withResolution(resolution)
            .withDomainSizeX(1.0)
            .makeShared();
    }
    auto flip = FlipSolver3::builder()
                    .withResolution(resolution)
                    .withDomainSizeX(1.0)
                    .makeShared();
    flip->setPicBlendingFactor(0.05);
    return flip;
}

void printUsage() {
    std::cerr << "Usage: hybrid_liquid <flip|apic|pic> <output-dir> [frames]\n"
              << "  Writes one OBJ of particle positions per frame.\n"
              << "  frames defaults to 40, at 1/60 s.\n";
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 3 && argc != 4) {
        printUsage();
        return 1;
    }

    const std::string method(argv[1]);
    if (method != "flip" && method != "apic" && method != "pic") {
        printUsage();
        return 1;
    }
    const std::string outputDir(argv[2]);
    int frameCount = 40;
    if (argc == 4) {
        frameCount = std::atoi(argv[3]);
        if (frameCount < 1) {
            std::cerr << "frames must be >= 1\n";
            return 1;
        }
    }
    if (!ensureDirectory(outputDir)) {
        return 1;
    }

    const Size3 resolution(32, 64, 32);
    PicSolver3Ptr solver = makeSolver(method, resolution);
    const double dx = 1.0 / static_cast<double>(resolution.x);
    const BoundingBox3D domain(Vector3D(),
                               Vector3D(1.0, dx * resolution.y, dx * resolution.z));

    auto container = Box3::builder()
                         .withIsNormalFlipped(true)
                         .withBoundingBox(domain)
                         .makeShared();
    solver->setCollider(
        RigidBodyCollider3::builder().withSurface(container).makeShared());

    auto liquid = Box3::builder()
                      .withLowerCorner(domain.lowerCorner + Vector3D(dx, dx, dx))
                      .withUpperCorner(Vector3D(0.40, 0.60 * domain.upperCorner.y,
                                                domain.upperCorner.z - dx))
                      .makeShared();
    solver->setParticleEmitter(VolumeParticleEmitter3::builder()
                                   .withSurface(liquid)
                                   .withSpacing(dx)
                                   .withIsOneShot(true)
                                   .makeShared());

    std::cout << method << " dam break, grid " << resolution.x << "x"
              << resolution.y << "x" << resolution.z << ", " << frameCount
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
