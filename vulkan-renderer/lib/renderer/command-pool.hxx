#pragma once
#include <lib/renderer/device.hxx>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class CommandPool {
  public:
    explicit CommandPool(const Device& device);

    auto getCommandPool() const -> const vk::raii::CommandPool& {
        return commandPool;
    }

  private:
    auto createCommandPool() -> vk::raii::CommandPool;

    const Device& device;
    vk::raii::CommandPool commandPool;
};

}  // namespace global_ao
