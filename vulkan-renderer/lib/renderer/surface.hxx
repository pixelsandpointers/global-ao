#pragma once
#include <lib/renderer/instance.hxx>
#include <vulkan/vulkan_raii.hpp>
#include <lib/window/window.hxx>

namespace global_ao {
class Surface {
  public:
    Surface(const Instance& instance, const Window& window);
    ;

    auto getVulkanInternalSurface() const -> const vk::raii::SurfaceKHR& {
        return surface;
    }

  private:
    static auto createSurface(const Instance& instance, const Window& window) -> vk::raii::SurfaceKHR;

    vk::raii::SurfaceKHR surface;
};
}  // namespace global_ao