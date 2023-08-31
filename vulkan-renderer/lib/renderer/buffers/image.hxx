#pragma once

#include <lib/renderer/device.hxx>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class Image {
  public:
    Image(
        const Device& device,
        vk::Format format,
        vk::Extent3D extent,
        vk::ImageTiling tiling,
        vk::ImageUsageFlags usage,
        vk::MemoryPropertyFlags properties,
        vk::ImageAspectFlags aspectFlags);

    auto getImage() const -> const vk::raii::Image&;
    auto getImageView() const -> const vk::raii::ImageView&;
    auto getFormat() const -> vk::Format;
    auto getExtent() const -> const vk::Extent3D&;
    auto getInitialLayout() -> vk::ImageLayout;

  private:
    auto createImage(const Device& device, vk::ImageTiling tiling, vk::ImageUsageFlags usage) -> vk::raii::Image;
    auto createImageView(const Device& device, vk::ImageAspectFlags aspectFlags) -> vk::raii::ImageView;

    vk::Format format;
    vk::Extent3D extent;
    vk::ImageLayout initialLayout;
    vk::DeviceSize imageSize;
    vk::raii::Image image;
    vk::raii::DeviceMemory imageMemory;
    vk::raii::ImageView imageView;
};

}  // namespace global_ao
