#pragma once
#include <lib/renderer/buffers/buffer.hxx>
#include <lib/renderer/device.hxx>
#include <lib/renderer/graphics-pipeline.hxx>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class CommandPool;

class CommandBuffers {
  public:
    CommandBuffers(const Device& device, size_t size, const CommandPool& commandPool);

    auto getCommandBuffer(size_t index) const -> const vk::raii::CommandBuffer&;
  private:
    auto createCommandBuffers() -> vk::raii::CommandBuffers;

    const size_t size;
    const Device& device;
    const CommandPool& commandPool;
    vk::raii::CommandBuffers commandBuffers;
};

}  // namespace global_ao
