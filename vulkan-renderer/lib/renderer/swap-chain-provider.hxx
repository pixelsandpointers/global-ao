#pragma once

#include <lib/renderer/device.hxx>
#include <lib/renderer/surface.hxx>
#include <lib/window/window.hxx>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class SwapChainHandler {
  public:
    SwapChainHandler(const Device& device, const Surface& surface, const Window& window);

    auto getExtent() const -> const vk::Extent2D& {
        return swapChainExtent;
    }

    auto getSurfaceFormat() const -> const vk::SurfaceFormatKHR& {
        return surfaceFormat;
    }

    auto getImageViews() const -> const std::vector<vk::raii::ImageView>& {
        return imageViews;
    }

    auto getSwapChain() const -> const vk::raii::SwapchainKHR& {
        return swapChain;
    }

  private:
    auto selectSurfaceFormat() -> vk::SurfaceFormatKHR;
    auto selectSwapExtent(const Window& window) -> vk::Extent2D;
    auto selectSwapPresentMode() -> vk::PresentModeKHR;
    auto createSwapChain() -> vk::raii::SwapchainKHR;
    auto createImageViews() -> std::vector<vk::raii::ImageView>;

    const Surface& surface;
    const Device& device;
    vk::SurfaceFormatKHR surfaceFormat;
    vk::Extent2D swapChainExtent;
    vk::raii::SwapchainKHR swapChain;
    std::vector<vk::Image> swapChainImages;
    std::vector<vk::raii::ImageView> imageViews;
};

}  // namespace global_ao
