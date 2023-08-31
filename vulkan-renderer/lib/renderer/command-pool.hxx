#pragma once
#include <lib/renderer/device.hxx>
#include <lib/renderer/command-buffers.hxx>

#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class CommandPool {
  public:
    explicit CommandPool(const Device& device);
    CommandPool(const Device& device, vk::CommandPoolCreateFlags additionalFlags);

    auto getCommandPool() const -> const vk::raii::CommandPool& {
        return commandPool;
    }

    auto createCommandBuffers(size_t size) const -> CommandBuffers;

  private:
    auto createCommandPool() -> vk::raii::CommandPool;
    auto createCommandPool(vk::CommandPoolCreateFlags additionalFlags) -> vk::raii::CommandPool;

    const Device& device;
    vk::raii::CommandPool commandPool;
};

}  // namespace global_ao
