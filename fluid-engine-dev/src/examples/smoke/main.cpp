#include "../example_io.h"

#include <jet/jet.h>

#include <cstdlib>
#include <iostream>
#include <string>

using namespace jet;

namespace {

void printUsage() {
    std::cerr << "Usage: smoke <output-dir> [frames]\n"
              << "  Writes a PPM of the mid-depth smoke-density slice per frame.\n"
              << "  frames defaults to 60, at 1/60 s.\n";
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 2 && argc != 3) {
        printUsage();
        return 1;
    }

    const std::string outputDir(argv[1]);
    int frameCount = 60;
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

    const Size3 resolution(32, 48, 32);
    auto solver = GridSmokeSolver3::builder()
                      .withResolution(resolution)
                      .withDomainSizeX(1.0)
                      .makeShared();
    solver->setClosedDomainBoundaryFlag(kDirectionAll & ~kDirectionUp);
    solver->setBuoyancyTemperatureFactor(5.0);

    const double dx = 1.0 / static_cast<double>(resolution.x);
    auto source = Sphere3::builder()
                      .withCenter({0.5, 4.0 * dx, 0.5})
                      .withRadius(2.5 * dx)
                      .makeShared();
    auto emitter = VolumeGridEmitter3::builder()
                       .withSourceRegion(source)
                       .withIsOneShot(false)
                       .makeShared();
    emitter->addStepFunctionTarget(solver->smokeDensity(), 0.0, 1.0);
    emitter->addStepFunctionTarget(solver->temperature(), 0.0, 1.0);
    solver->setEmitter(emitter);

    std::cout << "smoke plume, grid " << resolution.x << "x" << resolution.y
              << "x" << resolution.z << ", " << frameCount << " frames\n";

    for (Frame frame(0, 1.0 / 60.0); frame.index < frameCount; ++frame) {
        solver->update(frame);
        const std::string path = framePath(outputDir, frame.index, "ppm");
        if (!saveDensitySlicePpm(path, solver->smokeDensity())) {
            return 1;
        }
        std::cout << "frame " << frame.index << "  " << path << "\n";
    }
    return 0;
}
