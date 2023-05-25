set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/dependencies/dependency-sources)

# 1. Catch2 for testing
include(dependencies/setup_catch2)
# 2. OpenGL
include(dependencies/setup_opengl)
# 3. GLFW
include(dependencies/setup_glfw)
# 4. GLAD
include(dependencies/setup_glad)
# 5. ImGui for switches and reloads
include(dependencies/setup_imgui)
# 6. GLM for math
include(dependencies/setup_glm)

