cmake_minimum_required(VERSION 3.10)
project(Fluid3D)

# Native offline solver (exports particles_*.txt). Web demo uses wasm/ via Emscripten.
set(CMAKE_CXX_STANDARD 98)

include_directories(.)

set(EXECUTABLE_OUTPUT_PATH "bin")

add_executable(solver
    main.cpp
    fluidsim.cpp
    levelset_util.cpp
)
