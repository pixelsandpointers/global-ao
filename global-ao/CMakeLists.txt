add_executable(${CMAKE_PROJECT_NAME}-gao main.cxx)

target_sources(${CMAKE_PROJECT_NAME}-gao
    PRIVATE
    main.cxx
    BufferObject.hxx
    ShaderProgram.hxx
    Camera.hxx
    TriangleMesh.hxx TriangleMesh.cxx
    AOGeneratorCPU.hxx AOGeneratorCPU.cxx
    BVH.hxx BVH.cxx
    Compute.hxx Compute.cxx
    shader/test.vert shader/test.frag
    shader/AOComputeFull.comp
)

target_link_libraries(${CMAKE_PROJECT_NAME}-gao
    PRIVATE
    OpenGL::GL
    glad_gl_core_46
    glfw
    imgui_glfw
    glm
)
