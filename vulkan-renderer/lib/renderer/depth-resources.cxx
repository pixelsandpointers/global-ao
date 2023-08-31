#include <lib/renderer/depth-resources.hxx>

namespace global_ao {
DepthResources::DepthResources(const Device& device, vk::Extent2D extent)
  : extent {extent}, depthImage {
        device,
        findDepthFormat(device),
        vk::Extent3D {extent.width, extent.height, 1},
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        vk::ImageAspectFlagBits::eDepth
} {
}

auto DepthResources::getImageView() const -> const vk::raii::ImageView& {
    return depthImage.getImageView();
}

auto DepthResources::getImage() const -> const vk::raii::Image& {
    return depthImage.getImage();
}

auto DepthResources::findDepthFormat(const Device& device) -> vk::Format {
    return Utilities::findSupportedImageFormat(
        device,
        { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

auto DepthResources::getFormat() const -> vk::Format {
    return depthImage.getFormat();
}

auto DepthResources::getExtent() const -> const vk::Extent2D {
    const auto _extent = depthImage.getExtent();
    return { _extent.width, _extent.height };
}

}  // namespace global_ao