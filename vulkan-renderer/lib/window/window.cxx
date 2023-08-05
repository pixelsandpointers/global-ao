// clang-format off
#include <vulkan/vulkan_raii.hpp>
#include <GLFW/glfw3.h>
// clang-format on
#include <cstdint>
#include <iostream>
#include <lib/window/window.hxx>
#include <stdexcept>

namespace global_ao {

Window::Window(uint32_t width, uint32_t height)
  : window { createWindow("Global AO", width, height) } {
}

auto Window::getVulkanExtensions() const -> std::span<const char*> {
    uint32_t glfwExtensionCount = 0;
    auto* glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    return std::span<const char*> { glfwExtensions, glfwExtensionCount };
}

auto Window::createWindowSurface(const vk::raii::Instance& vulkanInstance) const -> VkSurfaceKHR {
    VkSurfaceKHR glfwSurface;
    auto err = glfwCreateWindowSurface(static_cast<VkInstance>(*vulkanInstance), window.get(), nullptr, &glfwSurface);
    if (err != 0) {
        std::cerr << ": Vulkan error " << vk::to_string(static_cast<vk::Result>(err));
        if (err < 0) {
            abort();
        }
    }
    return glfwSurface;
}

auto Window::createWindow(const std::string& title, std::size_t width, std::size_t height)
    -> std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)> {
    initialize();
    auto glfwWindow = std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)>(
        glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr),
        glfwDestroyWindow);
    if (!glfwWindow.get()) {
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwSetWindowUserPointer(window.get(), this);
    return glfwWindow;
}

auto Window::initialize() -> void {
    glfwSetErrorCallback(error_callback);
    auto success = glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if (success == 0) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
}

auto Window::error_callback(int error, const char* description) -> void {
    std::cerr << "GLFW error " << error << ": " << description << "\n";
}

auto Window::getFramebufferSize() const -> std::tuple<int, int> {
    int width, height;
    glfwGetFramebufferSize(window.get(), &width, &height);
    return { width, height };
}

}  // namespace global_ao