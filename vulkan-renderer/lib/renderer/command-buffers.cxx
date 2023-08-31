#include <lib/renderer/buffers/buffer.hxx>
#include <lib/renderer/command-buffers.hxx>
#include <lib/renderer/command-pool.hxx>

namespace global_ao {
CommandBuffers::CommandBuffers(const Device& device, size_t size, const CommandPool& commandPool)
  : size { size },
    device { device },
    commandPool { commandPool },
    commandBuffers { createCommandBuffers() } {
}

auto CommandBuffers::getCommandBuffer(const size_t index) const -> const vk::raii::CommandBuffer& {
    return commandBuffers.at(index);
}

auto CommandBuffers::createCommandBuffers() -> vk::raii::CommandBuffers {
    const auto& _commandPool = commandPool.getCommandPool();
    const auto commandBufferAllocateInfo =
        vk::CommandBufferAllocateInfo { .commandPool = *_commandPool,
                                        .level = vk::CommandBufferLevel::ePrimary,
                                        .commandBufferCount = static_cast<uint32_t>(size) };

    const auto& _device = device.getLogicalDevice();
    return { _device, commandBufferAllocateInfo };
}

}  // namespace global_ao