# Change the dependency sources directory to not be in the build directory
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/dependencies/dependency-sources)

include(dependencies/setup_catch2)
include(dependencies/setup_opengl)
include(dependencies/setup_glfw)
include(dependencies/setup_glad)
include(dependencies/setup_imgui)
include(dependencies/setup_glm)
include(dependencies/setup_stb)
include(dependencies/setup_assimp)
include(dependencies/setup_vulkan)
include(dependencies/setup_tinyobjloader)

