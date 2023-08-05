find_package(Vulkan REQUIRED)
target_compile_definitions(Vulkan::Vulkan INTERFACE -DVULKAN_HPP_NO_CONSTRUCTORS -DVULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)  # use C++20 aggregate initialization

if (NOT Vulkan_glslc_FOUND)
    message(FATAL_ERROR "glslc not found. Please install the Vulkan SDK.")
endif ()


function(global_ao_compile_shader target)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "ENV;FORMAT" "SOURCES")
    foreach (source ${arg_SOURCES})
        add_custom_command(
                OUTPUT ${source}.${arg_FORMAT}
                DEPENDS ${source}
                DEPFILE ${source}.d
                COMMAND
                ${Vulkan_GLSLC_EXECUTABLE}
                $<$<BOOL:${arg_ENV}>:--target-env=${arg_ENV}>
                $<$<BOOL:${arg_FORMAT}>:-mfmt=${arg_FORMAT}>
                -MD -MF ${source}.d
                -o ${source}.${arg_FORMAT}
                ${CMAKE_CURRENT_SOURCE_DIR}/${source}
        )

        set(SHADER_CUSTOM_TARGET "${CMAKE_PROJECT_NAME}-${target}-shader-${source}")
        add_custom_target(${SHADER_CUSTOM_TARGET} DEPENDS ${source}.${arg_FORMAT})
        add_dependencies(${target} ${SHADER_CUSTOM_TARGET})
        string(REPLACE "." "_" SHADER_COMPILE_DEFINITION ${source})
        target_compile_definitions(${target} PRIVATE "${SHADER_COMPILE_DEFINITION}_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/${source}.${arg_FORMAT}\"")
        message(STATUS "Target compile definition: ${SHADER_COMPILE_DEFINITION}_PATH=${CMAKE_CURRENT_BINARY_DIR}/${source}.${arg_FORMAT}")
    endforeach ()
endfunction()