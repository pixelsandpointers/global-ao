#include <lib/renderer/buffers/image.hxx>
#include <lib/renderer/utilities.hxx>

namespace global_ao {
Image::Image(
    const Device& device,
    vk::Format format,
    vk::Extent3D extent,
    vk::ImageTiling tiling,
    vk::ImageUsageFlags usage,
    vk::MemoryPropertyFlags properties,
    vk::ImageAspectFlags aspectFlags)
  : format { format },
    extent { extent },
    image { createImage(device, tiling, usage) },
    imageMemory { Utilities::createDeviceMemory(image, device, properties) },
    imageView { createImageView(device, aspectFlags) } {
}

auto Image::getImage() const -> const vk::raii::Image& {
    return image;
}

auto Image::getImageView() const -> const vk::raii::ImageView& {
    return imageView;
}

auto Image::getInitialLayout() -> vk::ImageLayout {
    return initialLayout;
}

auto Image::getExtent() const -> const vk::Extent3D& {
    return extent;
}

auto Image::getFormat() const -> vk::Format {
    return format;
}

auto Image::createImage(const Device& device, vk::ImageTiling tiling, vk::ImageUsageFlags usage) -> vk::raii::Image {
    const auto imageCreateInfo = vk::ImageCreateInfo {
        .imageType = vk::ImageType::e2D,
        .format = format,
        .extent = extent,
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = vk::SampleCountFlagBits::e1,
        .tiling = tiling,
        .usage = usage,
        .sharingMode = vk::SharingMode::eExclusive,
        .initialLayout = initialLayout,
    };
    return device.getLogicalDevice().createImage(imageCreateInfo);
}

auto Image::createImageView(const Device& device, vk::ImageAspectFlags aspectFlags) -> vk::raii::ImageView {
    const auto imageViewCreateInfo = vk::ImageViewCreateInfo {
        .image = *image,
        .viewType = vk::ImageViewType::e2D,
        .format = format,
        .subresourceRange = vk::ImageSubresourceRange {.aspectMask = aspectFlags,
                                                       .baseMipLevel = 0,
                                                       .levelCount = 1,
                                                       .baseArrayLayer = 0,
                                                       .layerCount = 1},
    };
    return device.getLogicalDevice().createImageView(imageViewCreateInfo);
}

}  // namespace global_ao
