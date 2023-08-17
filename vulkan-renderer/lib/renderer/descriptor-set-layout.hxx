#pragma once
#include <lib/renderer/device.hxx>
#include <vulkan/vulkan.hpp>

namespace global_ao {

class DescriptorSetLayout {
  public:
    explicit DescriptorSetLayout(const Device& device);

    auto getDescriptorSetLayout() const -> const vk::raii::DescriptorSetLayout&;

  private:
    auto createDescriptorSetLayout() -> vk::raii::DescriptorSetLayout;

    const Device& device;
    vk::raii::DescriptorSetLayout descriptorSetLayout;
    vk::DescriptorSetLayoutBinding samplerLayoutBinding;
};

}  // namespace global_ao
