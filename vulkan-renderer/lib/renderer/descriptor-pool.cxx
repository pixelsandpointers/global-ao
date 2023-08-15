#include <lib/renderer/descriptor-pool.hxx>
#include <lib/renderer/device.hxx>

namespace global_ao {
DescriptorPool::DescriptorPool(const Device& device, size_t descriptorCount)
  : descriptorCount { descriptorCount },
    descriptorPool { createDescriptorPool(device) } {
}

auto DescriptorPool::getDescriptorPool() const -> const vk::raii::DescriptorPool& {
    return descriptorPool;
}

auto DescriptorPool::createDescriptorPool(const Device& device) const -> vk::raii::DescriptorPool {
    const auto poolSizes =
        vk::DescriptorPoolSize { vk::DescriptorType::eUniformBuffer, static_cast<uint32_t>(descriptorCount) };

    const auto descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo {
        .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets = static_cast<uint32_t>(descriptorCount),
        .poolSizeCount = 1,
        .pPoolSizes = &poolSizes,
    };

    return vk::raii::DescriptorPool(device.getLogicalDevice(), descriptorPoolCreateInfo);
}

auto DescriptorPool::getDescriptorCount() const -> size_t {
    return descriptorCount;
}

}  // namespace global_ao