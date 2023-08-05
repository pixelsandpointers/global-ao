#pragma once
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
        const GraphicsPipeline& graphicsPipeline);

    auto getFrameBuffers() const -> const std::vector<vk::raii::Framebuffer>&;

    auto getFrameBuffer(size_t index) const -> const vk::raii::Framebuffer& {
        return frameBuffers[index];
    }

  private:
    auto createFrameBuffers() -> std::vector<vk::raii::Framebuffer>;

    const Device& device;
    const SwapChainHandler& imageViewProvider;
    const GraphicsPipeline& graphicsPipeline;
    std::vector<vk::raii::Framebuffer> frameBuffers;
};

}  // namespace global_ao
