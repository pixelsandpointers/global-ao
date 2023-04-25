add_executable(${CMAKE_PROJECT_NAME}-learn-opengl
    main.cxx
    learn-opengl.cxx
)

target_include_directories(${CMAKE_PROJECT_NAME}-learn-opengl
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${CMAKE_PROJECT_NAME}-learn-opengl
    PRIVATE
        glad_gl_core_mx_33 glfw
)