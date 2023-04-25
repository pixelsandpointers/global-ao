add_executable(${CMAKE_PROJECT_NAME}-window-test
        window.test.cxx
        )

target_link_libraries(${CMAKE_PROJECT_NAME}-window-test
        PRIVATE
        glad_gl_core_mx_33 glfw
        )