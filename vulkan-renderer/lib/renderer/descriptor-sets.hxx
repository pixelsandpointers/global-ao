#pragma once
#include <lib/renderer/buffers/uniform-buffer.hxx>
#include <lib/renderer/descriptor-pool.hxx>
#include <lib/renderer/descriptor-set-layout.hxx>
#include <lib/renderer/device.hxx>

namespace global_ao {

class DescriptorSets {
  public:
    DescriptorSets(
        const Device& device,
        const DescriptorPool& descriptorPool,
        const DescriptorSetLayout& descriptorSetLayout,
        const std::vector<UniformBuffer>& uniformBuffers);

    auto getDescriptorSets() const -> const vk::raii::DescriptorSets&;

  private:
    auto createInternalLayouts(const Device& device, const DescriptorPool& descriptorPool) const
        -> std::vector<vk::raii::DescriptorSetLayout>;
    auto createDescriptorSetLayoutHandles() const -> std::vector<vk::DescriptorSetLayout>;
    auto createDescriptorSets(
        const Device& device,
        const DescriptorPool& descriptorPool,
        const DescriptorSetLayout& descriptorSetLayout,
        const std::vector<UniformBuffer>& uniformBuffers) const -> vk::raii::DescriptorSets;
    auto populateDescriptorSets(const Device& device, const std::vector<UniformBuffer>& uniformBuffers) const -> void;

    std::vector<vk::raii::DescriptorSetLayout> internalLayouts;
    std::vector<vk::DescriptorSetLayout> descriptorSetLayoutHandles;
    vk::raii::DescriptorSets descriptorSets;
};

}  // namespace global_ao
