add_executable(${CMAKE_PROJECT_NAME}-vulkan-renderer-demo
        demo.cxx)

target_link_libraries(${CMAKE_PROJECT_NAME}-vulkan-renderer-demo PUBLIC ${CMAKE_PROJECT_NAME}-vulkan-renderer ${CMAKE_PROJECT_NAME}-window)

add_subdirectory(textures)