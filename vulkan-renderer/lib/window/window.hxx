#pragma once

// clang-format off
#include <vulkan/vulkan_raii.hpp>
#include <GLFW/glfw3.h>
// clang-format on
#include <memory>
#include <span>

namespace global_ao {

class Window {
  public:
    Window(uint32_t width, uint32_t height);

    auto getVulkanExtensions() const -> std::span<const char*>;
    auto createWindowSurface(const vk::raii::Instance& vulkanInstance) const -> VkSurfaceKHR;

    auto getWindow() const -> GLFWwindow* {
        return window.get();
    }

    auto getFramebufferSize() const -> std::tuple<int, int>;

  private:
    static auto error_callback(int error, const char* description) -> void;
    auto initialize() -> void;
    auto createWindow(const std::string& title, std::size_t width, std::size_t height)
        -> std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)>;

    std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)> window;
};

}  // namespace global_ao
