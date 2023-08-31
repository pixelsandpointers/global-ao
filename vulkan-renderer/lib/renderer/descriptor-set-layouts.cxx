#include <lib/renderer/buffer-objects/uniform-buffer-object.hxx>
#include <lib/renderer/buffers/image-sampler.hxx>
#include <lib/renderer/descriptor-set-layouts.hxx>

namespace global_ao {
DescriptorSetLayouts::DescriptorSetLayouts(
    size_t numberOfLayouts,
    const Device& device,
    std::span<vk::DescriptorSetLayoutBinding> bindings)
  : device { device },
    descriptorSetLayouts { createDescriptorSetLayouts(numberOfLayouts, bindings) } {
}

DescriptorSetLayouts::DescriptorSetLayouts(
    size_t numberOfLayouts,
    const Device& device,
    const std::vector<vk::DescriptorSetLayoutBinding>& bindings)
  : device { device },
    descriptorSetLayouts { createDescriptorSetLayouts(numberOfLayouts, bindings) } {
}

auto DescriptorSetLayouts::getDescriptorSetLayouts() const -> const std::vector<vk::raii::DescriptorSetLayout>& {
    return descriptorSetLayouts;
}

auto DescriptorSetLayouts::size() const -> size_t { return descriptorSetLayouts.size(); }

auto DescriptorSetLayouts::createDescriptorSetLayouts(
    size_t numberOfLayouts,
    std::span<vk::DescriptorSetLayoutBinding> bindings) -> std::vector<vk::raii::DescriptorSetLayout> {
    auto _descriptorSetLayouts = std::vector<vk::raii::DescriptorSetLayout> {};
    _descriptorSetLayouts.reserve(numberOfLayouts);

    for (size_t i = 0; i < numberOfLayouts; ++i) {
        auto descriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo {
            .bindingCount = static_cast<uint32_t>(bindings.size()),
            .pBindings = bindings.data(),
        };
        auto descriptorSetLayout = device.getLogicalDevice().createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
        _descriptorSetLayouts.emplace_back(std::move(descriptorSetLayout));
    }

    return _descriptorSetLayouts;
}

auto DescriptorSetLayouts::createDescriptorSetLayouts(
    size_t numberOfLayouts,
    const std::vector<vk::DescriptorSetLayoutBinding>& bindings) -> std::vector<vk::raii::DescriptorSetLayout> {
    auto _descriptorSetLayouts = std::vector<vk::raii::DescriptorSetLayout> {};
    _descriptorSetLayouts.reserve(numberOfLayouts);

    for (size_t i = 0; i < numberOfLayouts; ++i) {
        auto descriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo {
            .bindingCount = static_cast<uint32_t>(bindings.size()),
            .pBindings = bindings.data(),
        };
        auto descriptorSetLayout = device.getLogicalDevice().createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
        _descriptorSetLayouts.emplace_back(std::move(descriptorSetLayout));
    }

    return _descriptorSetLayouts;
}


}  // namespace global_ao