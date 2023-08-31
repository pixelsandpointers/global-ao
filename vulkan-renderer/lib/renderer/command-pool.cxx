#include <lib/renderer/command-pool.hxx>

namespace global_ao {
CommandPool::CommandPool(const Device& device)
  : device { device },
    commandPool { createCommandPool() } {
}

CommandPool::CommandPool(const Device& device, vk::CommandPoolCreateFlags additionalFlags)
  : device { device },
    commandPool { createCommandPool(additionalFlags) } {
}

auto CommandPool::createCommandPool() -> vk::raii::CommandPool {
    const auto graphicsQueueFamilyIndex = device.getGraphicsQueueFamilyIndex();

    const auto commandPoolCreateInfo = vk::CommandPoolCreateInfo {
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = graphicsQueueFamilyIndex,

    };
    return { device.getLogicalDevice(), commandPoolCreateInfo };
}

auto CommandPool::createCommandPool(vk::CommandPoolCreateFlags additionalFlags) -> vk::raii::CommandPool {
    const auto graphicsQueueFamilyIndex = device.getGraphicsQueueFamilyIndex();

    const auto commandPoolCreateInfo = vk::CommandPoolCreateInfo {
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer | additionalFlags,
        .queueFamilyIndex = graphicsQueueFamilyIndex,

    };
    return { device.getLogicalDevice(), commandPoolCreateInfo };
}

auto CommandPool::createCommandBuffers(size_t size) const -> CommandBuffers {
    const auto& thisReference = *this;
    return { device, size, thisReference };
}

}  // namespace global_ao