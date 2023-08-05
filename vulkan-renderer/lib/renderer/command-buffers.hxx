#pragma once
#include <lib/renderer/command-pool.hxx>
#include <lib/renderer/device.hxx>
#include <lib/renderer/graphics-pipeline.hxx>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class CommandBuffers {
  public:
    CommandBuffers(size_t size, const Device& device, const CommandPool& commandPool);

    auto getCommandBuffer(size_t index) const -> const vk::raii::CommandBuffer&;
    auto recordCommandBuffer(
        const vk::raii::CommandBuffer& commandBuffer,
        const GraphicsPipeline& pipeline,
        const vk::Framebuffer& frameBuffer,
        const vk::Extent2D& extent) -> void;

  private:
    auto createCommandBuffers() -> vk::raii::CommandBuffers;

    const size_t size;
    const Device& device;
    const CommandPool& commandPool;
    vk::raii::CommandBuffers commandBuffers;
};

}  // namespace global_ao
