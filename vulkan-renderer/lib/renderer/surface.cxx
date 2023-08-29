#include <lib/renderer/surface.hxx>

namespace global_ao {

Surface::Surface(const Instance& instance, const Window& window)
  : surface { createSurface(instance, window) } {
}

auto Surface::createSurface(const Instance& instance, const Window& window) -> vk::raii::SurfaceKHR {
    auto* windowSurface = window.createWindowSurface(instance.getVulkanInternalInstance());
    return { instance.getVulkanInternalInstance(), windowSurface };
}
}  // namespace global_ao