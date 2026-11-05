#
# Copyright (c) 2019 Doyub Kim
#
# I am making my contributions/submissions to this project solely in my personal
# capacity and am not conveying any rights to any intellectual property of any
# third parties.
#

# CMake version
cmake_minimum_required(VERSION 3.10)

# Include cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Declare project
project(jet)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Includes
include_directories(include)
include_directories(external/tinyobj)

# Build type - Release by default
message("CMake build type: " ${CMAKE_BUILD_TYPE})
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DDEBUG)
endif()

# Configure tasking system backend
include(cmake/TaskingSystemOptions.cmake)

# Compile options
include(cmake/CompileOptions.cmake)

# Overrides
set(CMAKE_MACOSX_RPATH ON)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
add_subdirectory(src/jet)
add_subdirectory(src/examples)
