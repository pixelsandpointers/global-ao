add_executable(gao-testing)

file(GLOB headers ../global-ao/*.hxx)
file(GLOB source ../global-ao/*.cxx)
file(GLOB vertShaders shader/*.vert)
file(GLOB fragShaders shader/*.frag)

list(REMOVE_ITEM source "${CMAKE_SOURCE_DIR}/global-ao-testing/../global-ao/main.cxx")
list(APPEND source "${CMAKE_SOURCE_DIR}/global-ao-testing/test.cxx")

target_sources(gao-testing
        PRIVATE
        ${headers}
        ${source}
        ${vertShaders}
        ${fragShaders}
    )

target_include_directories(gao-testing PRIVATE ${stb_SOURCE_DIR})
target_include_directories(gao-testing PRIVATE "${CMAKE_SOURCE_DIR}/global-ao")
target_link_libraries(gao-testing
        PRIVATE
        OpenGL::GL
        glad_gl_core_46
        glfw
        imgui_glfw
        glm
        assimp
    )
