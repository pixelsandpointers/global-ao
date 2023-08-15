#include <lib/renderer/buffers/uniform-buffer-object.hxx>
#include <lib/renderer/descriptor-set-layout.hxx>

namespace global_ao {
DescriptorSetLayout::DescriptorSetLayout(const Device& device)
  : device { device },
    descriptorSetLayout { createDescriptorSetLayout() } {
}

auto DescriptorSetLayout::createDescriptorSetLayout() -> vk::raii::DescriptorSetLayout {
    const auto descriptorSetLayoutBinding = UniformBufferObject::getBindingDescription();
    auto descriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo {
        .bindingCount = 1,
        .pBindings = &descriptorSetLayoutBinding,
    };
    return device.getLogicalDevice().createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
}

auto DescriptorSetLayout::getDescriptorSetLayout() const -> const vk::raii::DescriptorSetLayout& {
    return descriptorSetLayout;
}
}  // namespace global_ao