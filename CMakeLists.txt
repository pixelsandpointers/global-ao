# Keep the version number as low as possible as to prevent other projects from being forced to require a more
# recent version than their target environment typically allows or provides.
cmake_minimum_required(VERSION 3.25)

# Use semantic versioning for the project (https://semver.org/).
project(global_ao
        VERSION 0.1.0
        LANGUAGES CXX
        )

# Set up dependencies
# 1. Catch2 for testing
include(dependencies/fetch_catch2.cmake)

add_subdirectory(global-ao)
add_subdirectory(tests)
