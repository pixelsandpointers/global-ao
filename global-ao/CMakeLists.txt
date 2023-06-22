add_executable(gao App.cxx)

file(GLOB headers *.hxx)
file(GLOB source *.cxx)
file(GLOB shaders shader/*.glsl)

target_sources(gao
        PRIVATE
        ${headers}
        ${source}
        ${shaders}
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
