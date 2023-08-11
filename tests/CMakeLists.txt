global_ao_add_test(microbenchtest microbenchtest.test.cxx)

global_ao_add_test(test test.cxx)

global_ao_add_test(window
        window.test.cxx
        LINK_LIBRARIES
        glad_gl_core_46
        glfw
        )

global_ao_add_test(glm
        glm.test.cxx
        LINK_LIBRARIES
        glm
        )

global_ao_add_test(dear-imgui
        dear-imgui.test.cxx
        LINK_LIBRARIES
        imgui_glfw
        OpenGL::GL
        )