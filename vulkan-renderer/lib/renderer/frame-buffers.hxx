#pragma once
#include <lib/renderer/depth-resources.hxx>
#include <lib/renderer/device.hxx>
#include <lib/renderer/graphics-pipeline.hxx>
#include <lib/renderer/swap-chain-provider.hxx>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class FrameBuffers {
  public:
    FrameBuffers(
        const Device& device,
        const SwapChainHandler& imageViewProvider,
        const DepthResources& depthResources,
        const GraphicsPipeline& graphicsPipeline);

    FrameBuffers(const Device& device, const DepthResources& depthResources, const GraphicsPipeline& graphicsPipeline);

    FrameBuffers(
        const Device& device,
        const vk::raii::ImageView& imageView,
        vk::Extent2D extent,
        const GraphicsPipeline& graphicsPipeline);

    FrameBuffers(const Device& device, const std::span<const Image> images, const GraphicsPipeline& graphicsPipeline);

    auto getFrameBuffers() const -> const std::vector<vk::raii::Framebuffer>&;

    auto getFrameBuffer(size_t index) const -> const vk::raii::Framebuffer& {
        return frameBuffers[index];
    }

  private:
    auto createFrameBuffers(const DepthResources& depthResources, const SwapChainHandler& imageViewProvider)
        -> std::vector<vk::raii::Framebuffer>;
    auto createFrameBuffers(const DepthResources& depthResources) -> std::vector<vk::raii::Framebuffer>;
    auto createFrameBuffers(const vk::raii::ImageView& imageView, vk::Extent2D extent)
        -> std::vector<vk::raii::Framebuffer>;
    auto createFrameBuffers(const std::span<const Image> images)
    -> std::vector<vk::raii::Framebuffer>;

    const Device& device;
    ;
    const GraphicsPipeline& graphicsPipeline;
    std::vector<vk::raii::Framebuffer> frameBuffers;
};

}  // namespace global_ao
