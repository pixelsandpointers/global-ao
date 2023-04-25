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

# Set up dependencies
# 1. Catch2 for testing
include(dependencies/setup_catch2.cmake)
# 2. OpenGL
include(dependencies/setup_opengl.cmake)
# 3. GLFW
include(dependencies/setup_glfw.cmake)
# 4. GLAD
include(dependencies/setup_glad.cmake)

add_subdirectory(global-ao)
add_subdirectory(tests)
add_subdirectory(learnings)
