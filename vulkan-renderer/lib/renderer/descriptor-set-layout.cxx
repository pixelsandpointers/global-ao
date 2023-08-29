#include <lib/renderer/buffers/uniform-buffer-object.hxx>
#include <lib/renderer/descriptor-set-layout.hxx>
#include <lib/renderer/textures/texture-sampler.hxx>

namespace global_ao {
DescriptorSetLayout::DescriptorSetLayout(const Device& device)
  : device { device },
    descriptorSetLayout { createDescriptorSetLayout() } {
}

auto DescriptorSetLayout::getDescriptorSetLayout() const -> const vk::raii::DescriptorSetLayout& {
    return descriptorSetLayout;
}

auto DescriptorSetLayout::createDescriptorSetLayout() -> vk::raii::DescriptorSetLayout {
    const auto descriptorSetLayoutBindings =
        std::array { UniformBufferObject::getBindingDescription(), TextureSampler::getBindingDescription() };
    auto descriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo {
        .bindingCount = descriptorSetLayoutBindings.size(),
        .pBindings = descriptorSetLayoutBindings.data(),
    };
    return device.getLogicalDevice().createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
}

}  // namespace global_ao