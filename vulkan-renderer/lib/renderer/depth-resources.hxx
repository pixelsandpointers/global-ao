#pragma once
#include <lib/renderer/buffers/image.hxx>
#include <lib/renderer/device.hxx>
#include <lib/renderer/utilities.hxx>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class DepthResources {
  public:
    DepthResources(const Device& device, vk::Extent2D extent);

    auto getFormat() const -> vk::Format {
        return depthImage.getFormat();
    }

    auto getImageView() const -> const vk::raii::ImageView&;

  private:
    static auto findDepthFormat(const Device& device) -> vk::Format;

    Image depthImage;
};

}  // namespace global_ao
