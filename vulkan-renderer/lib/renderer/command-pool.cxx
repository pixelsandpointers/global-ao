#include <lib/renderer/command-pool.hxx>

namespace global_ao {
CommandPool::CommandPool(const Device& device)
  : device { device },
    commandPool { createCommandPool() } {
}

auto CommandPool::createCommandPool() -> vk::raii::CommandPool {
    const auto graphicsQueueFamilyIndex = device.getGraphicsQueueFamilyIndex();

    const auto commandPoolCreateInfo = vk::CommandPoolCreateInfo {
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = graphicsQueueFamilyIndex,

    };
    return { device.getLogicalDevice(), commandPoolCreateInfo };
}

}  // namespace global_ao