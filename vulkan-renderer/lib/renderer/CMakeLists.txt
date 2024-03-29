add_library(${CMAKE_PROJECT_NAME}-vulkan-renderer
        device.cxx
        dynamic-loader.cxx
        instance.cxx
        renderer.cxx
        surface.cxx
        swap-chain-provider.cxx
        graphics-pipeline.cxx
        shader-module.cxx
        frame-buffers.cxx
        command-pool.cxx
        command-buffers.cxx
        sync-objects-handler.cxx
        descriptor-set-layouts.cxx
        descriptor-pool.cxx
        utilities.cxx
        depth-resources.cxx
)

target_link_libraries(${CMAKE_PROJECT_NAME}-vulkan-renderer PUBLIC
        Vulkan::Vulkan
        glfw
        glm
        stb
        ${CMAKE_PROJECT_NAME}-window)

target_include_directories(${CMAKE_PROJECT_NAME}-vulkan-renderer
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}../../
)

global_ao_add_test(vulkan-renderer-test
        renderer.test.cxx
        LINK_LIBRARIES
        ${CMAKE_PROJECT_NAME}-vulkan-renderer
)

add_subdirectory(ambient-occlusion)
add_subdirectory(buffers)
add_subdirectory(shaders)
add_subdirectory(textures)
add_subdirectory(buffer-objects)