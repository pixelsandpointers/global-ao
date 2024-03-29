Include(FetchContent)

FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad
        GIT_TAG d08b1aa  # v2.0.4
        SOURCE_SUBDIR cmake
)

FetchContent_GetProperties(glad)
set(GLAD_PROFILE "core" CACHE STRING "OpenGL profile")

set(GLAD_GENERATOR "c" CACHE STRING "Language to generate the binding for")

FetchContent_MakeAvailable(glad)

# add the library with given options
# check https://github.com/Dav1dde/glad/blob/glad2/cmake/CMakeLists.txt for more information
glad_add_library(glad_gl_core_46 REPRODUCIBLE API gl:core=4.6)

