#include <lib/renderer/frame-buffer.hxx>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {
FrameBuffers::FrameBuffers(
    const Device& device,
    const SwapChainHandler& imageViewProvider,
    const DepthResources& depthResources,
    const GraphicsPipeline& graphicsPipeline)
  : device { device },
    imageViewProvider { imageViewProvider },
    graphicsPipeline { graphicsPipeline },
    frameBuffers { createFrameBuffers(depthResources) } {
}

auto FrameBuffers::createFrameBuffers(const DepthResources& depthResources) -> std::vector<vk::raii::Framebuffer> {
    const auto& imageViews = imageViewProvider.getImageViews();
    const auto& extent = imageViewProvider.getExtent();
    const auto& renderPass = graphicsPipeline.getRenderPass();

    std::vector<vk::raii::Framebuffer> _frameBuffers;
    _frameBuffers.reserve(imageViews.size());

    for (const auto& imageView : imageViews) {
        const auto attachments = std::vector<vk::ImageView> { *imageView, *depthResources.getImageView() };

        const auto framebufferCreateInfo = vk::FramebufferCreateInfo {
            .renderPass = *renderPass,
            .attachmentCount = static_cast<uint32_t>(attachments.size()),
            .pAttachments = attachments.data(),
            .width = extent.width,
            .height = extent.height,
            .layers = 1,
        };

        _frameBuffers.emplace_back(device.getLogicalDevice(), framebufferCreateInfo);
    }

    return _frameBuffers;
}

auto FrameBuffers::getFrameBuffers() const -> const std::vector<vk::raii::Framebuffer>& {
    return frameBuffers;
}
}  // namespace global_ao