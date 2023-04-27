include(FetchContent)

FetchContent_Declare(
        ImGui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG        v1.89.5
)

FetchContent_MakeAvailable(ImGui)


# example setup see: https://github.com/ocornut/imgui/blob/master/examples/example_glfw_opengl3/Makefile
add_library(imgui_glfw STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui.h
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp

    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    # only keep if we want to build stuff in vulkan
    #imgui/backends/imgui_impl_vulkan.cpp
)

target_link_libraries(imgui_glfw PUBLIC glfw ${OPENGL_LIBRARIES})

target_include_directories(imgui_glfw
	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/imgui
	${CMAKE_CURRENT_LIST_DIR}/imgui/backends
)
