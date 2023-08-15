#pragma once
#include <lib/renderer/device.hxx>
#include <vulkan/vulkan.hpp>

namespace global_ao {

class DescriptorSetLayout {
  public:
    explicit DescriptorSetLayout(const Device& device);

    auto getDescriptorSetLayout() const -> const vk::raii::DescriptorSetLayout&;

  private:
    auto createDescriptorSetLayoutBinding() -> vk::DescriptorSetLayoutBinding;
    auto createDescriptorSetLayout() -> vk::raii::DescriptorSetLayout;

    const Device& device;
    vk::raii::DescriptorSetLayout descriptorSetLayout;
};

}  // namespace global_ao
