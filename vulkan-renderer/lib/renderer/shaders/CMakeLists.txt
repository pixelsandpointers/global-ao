global_ao_compile_shader(${CMAKE_PROJECT_NAME}-vulkan-renderer
        ENV vulkan1.3
        FORMAT bin
        SOURCES
        shader.vert
        shader.frag
        )

