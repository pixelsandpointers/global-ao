#include <lib/renderer/depth-resources.hxx>

namespace global_ao {
DepthResources::DepthResources(const Device& device, vk::Extent2D extent)
  : depthImage {
        device,
        findDepthFormat(device),
        vk::Extent3D {extent.width, extent.height, 1},
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        vk::ImageAspectFlagBits::eDepth
} {
}

auto DepthResources::getImageView() const -> const vk::raii::ImageView& {
    return depthImage.getImageView();
}

auto DepthResources::findDepthFormat(const Device& device) -> vk::Format {
    return Utilities::findSupportedImageFormat(
        device,
        { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

}  // namespace global_ao