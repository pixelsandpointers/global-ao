#pragma once
#include <lib/renderer/device.hxx>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class DescriptorPool {
  public:
    DescriptorPool(const Device& device, size_t descriptorCount);

    auto getDescriptorPool() const -> const vk::raii::DescriptorPool&;
    auto getDescriptorCount() const -> size_t;

  private:
    auto createDescriptorPool(const Device& device) const -> vk::raii::DescriptorPool;

    size_t descriptorCount;
    vk::raii::DescriptorPool descriptorPool;
};

}  // namespace global_ao
