#pragma once
#include <lib/renderer/buffers/image.hxx>
#include <lib/renderer/device.hxx>
#include <lib/renderer/utilities.hxx>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class DepthResources {
  public:
    DepthResources(const Device& device, vk::Extent2D extent);

    auto getFormat() const -> vk::Format;

    auto getExtent() const -> const vk::Extent2D;

    auto getImageView() const -> const vk::raii::ImageView&;

    auto getImage() const -> const vk::raii::Image&;

  private:
    static auto findDepthFormat(const Device& device) -> vk::Format;

    vk::Extent2D extent;
    Image depthImage;
};

}  // namespace global_ao
