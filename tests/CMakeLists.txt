global_ao_add_test(window
        window.test.cxx
        LIBRARIES
        glad_gl_core_46
        glfw
        )

global_ao_add_test(glm
        glm.test.cxx
        LIBRARIES
        glm
        )

global_ao_add_test(dear-imgui
        dear-imgui.test.cxx
        LIBRARIES
        imgui_glfw
        OpenGL::GL
        )
