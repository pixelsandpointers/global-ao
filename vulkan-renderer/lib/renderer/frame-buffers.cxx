#include <lib/renderer/frame-buffers.hxx>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {
FrameBuffers::FrameBuffers(
    const Device& device,
    const SwapChainHandler& imageViewProvider,
    const DepthResources& depthResources,
    const GraphicsPipeline& graphicsPipeline)
  : device { device },
    graphicsPipeline { graphicsPipeline },
    frameBuffers { createFrameBuffers(depthResources, imageViewProvider) } {
}

FrameBuffers::FrameBuffers(
    const Device& device,
    const DepthResources& depthResources,
    const GraphicsPipeline& graphicsPipeline)
  : device { device },
    graphicsPipeline { graphicsPipeline },
    frameBuffers { createFrameBuffers(depthResources) } {
}

FrameBuffers::FrameBuffers(
    const Device& device,
    const vk::raii::ImageView& imageView,
    vk::Extent2D extent,
    const GraphicsPipeline& graphicsPipeline)
  : device { device },
    graphicsPipeline { graphicsPipeline },
    frameBuffers { createFrameBuffers(imageView, extent) } {
}

FrameBuffers::FrameBuffers(
    const Device& device,
    const std::span<const Image> images,
    const GraphicsPipeline& graphicsPipeline)
  : device { device },
    graphicsPipeline { graphicsPipeline },
    frameBuffers { createFrameBuffers(images) }  {
}

auto FrameBuffers::getFrameBuffers() const -> const std::vector<vk::raii::Framebuffer>& {
    return frameBuffers;
}

auto FrameBuffers::createFrameBuffers(const DepthResources& depthResources, const SwapChainHandler& imageViewProvider)
    -> std::vector<vk::raii::Framebuffer> {
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

auto FrameBuffers::createFrameBuffers(const DepthResources& depthResources) -> std::vector<vk::raii::Framebuffer> {
    const auto extent = depthResources.getExtent();
    const auto& renderPass = graphicsPipeline.getRenderPass();

    auto _frameBuffers = std::vector<vk::raii::Framebuffer>();
    _frameBuffers.reserve(1);

    const auto attachments = std::vector<vk::ImageView> { *depthResources.getImageView() };
    const auto framebufferCreateInfo = vk::FramebufferCreateInfo {
        .renderPass = *renderPass,
        .attachmentCount = 1,
        .pAttachments = attachments.data(),
        .width = extent.width,
        .height = extent.height,
        .layers = 1,
    };
    _frameBuffers.emplace_back(device.getLogicalDevice(), framebufferCreateInfo);

    return _frameBuffers;
}

auto FrameBuffers::createFrameBuffers(const vk::raii::ImageView& imageView, vk::Extent2D extent)
    -> std::vector<vk::raii::Framebuffer> {
    const auto& renderPass = graphicsPipeline.getRenderPass();

    std::vector<vk::raii::Framebuffer> _frameBuffers;
    _frameBuffers.reserve(1);

    const auto framebufferCreateInfo = vk::FramebufferCreateInfo {
        .renderPass = *renderPass,
        .attachmentCount = 1,
        .pAttachments = &*imageView,
        .width = extent.width,
        .height = extent.height,
        .layers = 1,
    };

    _frameBuffers.emplace_back(device.getLogicalDevice(), framebufferCreateInfo);

    return _frameBuffers;
}

auto FrameBuffers::createFrameBuffers(const std::span<const Image> images) -> std::vector<vk::raii::Framebuffer> {
    const auto& renderPass = graphicsPipeline.getRenderPass();

    std::vector<vk::raii::Framebuffer> _frameBuffers;
    _frameBuffers.reserve(images.size());

    for (const auto& image : images) {
        const auto attachments = std::vector<vk::ImageView> { *image.getImageView() };
        const auto& extent = image.getExtent();

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

}  // namespace global_ao