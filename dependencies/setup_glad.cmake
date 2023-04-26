Include(FetchContent)

FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad
        GIT_TAG d08b1aa  # v2.0.4
        FIND_PACKAGE_ARGS
)

FetchContent_GetProperties(glad)
set(GLAD_PROFILE "core" CACHE STRING "OpenGL profile")

set(GLAD_GENERATOR "c" CACHE STRING "Language to generate the binding for")

FetchContent_MakeAvailable(glad)

add_subdirectory(${glad_SOURCE_DIR}/cmake glad_cmake)

# add the library with given options
# check https://github.com/Dav1dde/glad/blob/glad2/cmake/CMakeLists.txt for more information
glad_add_library(glad_gl_core_33 REPRODUCIBLE API gl:core=3.3)

