global_ao_add_test(test test.cxx)

global_ao_add_test(microbenchtest microbenchtest.test.cxx)

file(GLOB headers *.hxx)
file(GLOB source *.cxx)
file(GLOB shaders shader/*)

add_executable(gao App.cxx)

target_sources(gao
        PRIVATE
        ${headers}
        ${source}
        ${shaders}
    )

target_link_libraries(gao
        PRIVATE
        Catch2
        OpenGL::GL
        glad_gl_core_46
        glfw
        imgui_glfw
        glm
    )
