add_executable(gao main.cxx)

target_sources(gao
    PRIVATE
    main.cxx
    BufferObject.hxx
    ShaderProgram.hxx
    Camera.hxx
    TriangleMesh.hxx TriangleMesh.cxx
    shader/test.vert shader/test.frag
)

target_link_libraries(${CMAKE_PROJECT_NAME}-gao
    PRIVATE
    OpenGL::GL
    glad_gl_core_46
    glfw
    imgui_glfw
    glm
)
