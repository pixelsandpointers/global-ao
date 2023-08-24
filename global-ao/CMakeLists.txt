add_executable(gao)

file(GLOB headers *.hxx)
file(GLOB source *.cxx)
file(GLOB vertShaders shader/*.vert)
file(GLOB fragShaders shader/*.frag)

source_group("Shader Files" FILES ${vertShaders} ${fragShaders})

target_sources(gao
        PRIVATE
        ${headers}
        ${source}
        ${vertShaders}
        ${fragShaders}
    )

target_include_directories(gao PRIVATE ${stb_SOURCE_DIR})
target_link_libraries(gao
        PRIVATE
        OpenGL::GL
        glad_gl_core_46
        glfw
        imgui_glfw
        glm
        assimp
    )
