# Keep the version number as low as possible as to prevent other projects from being forced to require a more
# recent version than their target environment typically allows or provides.
cmake_minimum_required(VERSION 3.25)

# Use semantic versioning for the project (https://semver.org/).
project(global-ao
        VERSION 0.1.0
        LANGUAGES CXX C
        )

# Set the C++ standard to C++20 and disable extensions for all targets
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the CMAKE_MODULE_PATH to the cmake directory in the project root
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

# Debug Settings
include(debug_settings)

# Set up dependencies
include(setup_dependencies)

# Setup tests
include(setup_tests)
add_subdirectory(tests)

# Add the global-ao library
add_subdirectory(global-ao)

add_subdirectory(vulkan-renderer)
add_subdirectory(vulkan-renderer-demo)
