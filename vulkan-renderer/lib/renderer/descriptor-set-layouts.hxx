#pragma once
#include <lib/renderer/device.hxx>
#include <vulkan/vulkan.hpp>

namespace global_ao {

class DescriptorSetLayouts {
  public:
    DescriptorSetLayouts(
        size_t numberOfLayouts,
        const Device& device,
        std::span<vk::DescriptorSetLayoutBinding> bindings);
    DescriptorSetLayouts(
        size_t numberOfLayouts,
        const Device& device,
        const std::vector<vk::DescriptorSetLayoutBinding>& bindings);

    auto getDescriptorSetLayouts() const -> const std::vector<vk::raii::DescriptorSetLayout>&;
    auto size() const -> size_t;

  private:
    auto createDescriptorSetLayouts(size_t numberOfLayouts, std::span<vk::DescriptorSetLayoutBinding> bindings)
        -> std::vector<vk::raii::DescriptorSetLayout>;
    auto createDescriptorSetLayouts(size_t numberOfLayouts, const std::vector<vk::DescriptorSetLayoutBinding>& bindings)
        -> std::vector<vk::raii::DescriptorSetLayout>;

    const Device& device;
    const std::vector<vk::raii::DescriptorSetLayout> descriptorSetLayouts;
    vk::DescriptorSetLayoutBinding samplerLayoutBinding;
};

}  // namespace global_ao
