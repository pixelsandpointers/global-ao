add_executable(${CMAKE_PROJECT_NAME}-window-test
        window.test.cxx
        )

target_link_libraries(${CMAKE_PROJECT_NAME}-window-test
        PRIVATE
        glad_gl_core_46 glfw Catch2::Catch2WithMain
        )

add_executable(${CMAKE_PROJECT_NAME}-glm-test
        glm.test.cxx
        )

target_link_libraries(${CMAKE_PROJECT_NAME}-glm-test
        PRIVATE
        glm Catch2::Catch2WithMain
        )

add_executable(${CMAKE_PROJECT_NAME}-dear-imgui-test
        dear-imgui.test.cxx
        )

target_link_libraries(${CMAKE_PROJECT_NAME}-dear-imgui-test
        PRIVATE
        glad_gl_core_46 glfw imgui_glfw OpenGL::GL Catch2::Catch2WithMain
        )