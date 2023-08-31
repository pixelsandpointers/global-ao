add_library(${CMAKE_PROJECT_NAME}-window
        window.cxx)

target_link_libraries(${CMAKE_PROJECT_NAME}-window PUBLIC
        glfw
        PRIVATE
        Vulkan::Vulkan
        ${CMAKE_PROJECT_NAME}-vulkan-renderer
        )

target_include_directories(${CMAKE_PROJECT_NAME}-window
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../
        )

