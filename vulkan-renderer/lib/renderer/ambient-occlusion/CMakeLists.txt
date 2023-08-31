target_sources(${CMAKE_PROJECT_NAME}-vulkan-renderer
        PUBLIC
        ambient-occlusion.cxx
)

add_subdirectory(pipelines)