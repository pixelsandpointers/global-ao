#include <lib/renderer/buffers/uniform-buffer-object.hxx>
#include <lib/renderer/buffers/uniform-buffer.hxx>
#include <lib/renderer/descriptor-sets.hxx>
#include <lib/renderer/textures/texture-image.hxx>
#include <lib/renderer/textures/texture-sampler.hxx>

namespace global_ao {
DescriptorSets::DescriptorSets(
    const Device& device,
    const DescriptorPool& descriptorPool,
    const DescriptorSetLayout& descriptorSetLayout,
    const std::vector<UniformBuffer>& uniformBuffers,
    const TextureImage& textureImage,
    const TextureSampler& textureSampler)
  : internalLayouts { createInternalLayouts(device, descriptorPool) },
    descriptorSetLayoutHandles { createDescriptorSetLayoutHandles() },
    descriptorSets {
        createDescriptorSets(device, descriptorPool, descriptorSetLayout, uniformBuffers, textureImage, textureSampler)
    } {
}

auto DescriptorSets::getDescriptorSets() const -> const vk::raii::DescriptorSets& {
    return descriptorSets;
}

auto DescriptorSets::createInternalLayouts(const Device& device, const DescriptorPool& descriptorPool) const
    -> std::vector<vk::raii::DescriptorSetLayout> {
    // TODO: this is not ideal, but will be fine for now
    //       we are creating a new descriptor set layout for each uniform buffer object
    //       instead of reusing the descriptor set layout class
    auto _internalLayouts = std::vector<vk::raii::DescriptorSetLayout> {};
    _internalLayouts.reserve(descriptorPool.getDescriptorCount());
    const auto descriptorSetLayoutBindings =
        std::array { UniformBufferObject::getBindingDescription(), TextureSampler::getBindingDescription() };

    for (auto i = 0; i < descriptorPool.getDescriptorCount(); ++i) {
        auto descriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo {
            .bindingCount = descriptorSetLayoutBindings.size(),
            .pBindings = descriptorSetLayoutBindings.data(),
        };
        _internalLayouts.push_back(device.getLogicalDevice().createDescriptorSetLayout(descriptorSetLayoutCreateInfo));
    }

    return _internalLayouts;
}

auto DescriptorSets::createDescriptorSetLayoutHandles() const -> std::vector<vk::DescriptorSetLayout> {
    auto _descriptorSetLayoutHandles = std::vector<vk::DescriptorSetLayout> {};
    _descriptorSetLayoutHandles.reserve(internalLayouts.size());

    for (const auto& internalLayout : internalLayouts) {
        _descriptorSetLayoutHandles.push_back(*internalLayout);
    }
    return _descriptorSetLayoutHandles;
}

auto DescriptorSets::createDescriptorSets(
    const Device& device,
    const DescriptorPool& descriptorPool,
    const DescriptorSetLayout& descriptorSetLayout,  // TODO: this function should make use of this parameter
    const std::vector<UniformBuffer>& uniformBuffers,
    const TextureImage& textureImage,
    const TextureSampler& textureSampler) const -> vk::raii::DescriptorSets {
    auto descriptorSetAllocateInfo = vk::DescriptorSetAllocateInfo {
        .descriptorPool = *descriptorPool.getDescriptorPool(),
        .descriptorSetCount = static_cast<uint32_t>(descriptorPool.getDescriptorCount()),
        .pSetLayouts = descriptorSetLayoutHandles.data(),
    };

    auto _descriptorSets = vk::raii::DescriptorSets { device.getLogicalDevice(), descriptorSetAllocateInfo };

    // configure descriptor sets

    for (auto i = 0; i < uniformBuffers.size(); ++i) {
        const auto& descriptorSet = _descriptorSets[i];
        const auto& uniformBuffer = uniformBuffers[i];

        auto descriptorBufferInfo = vk::DescriptorBufferInfo { .buffer = *uniformBuffer.getBuffer().getBuffer(),
                                                               .offset = 0,
                                                               .range = sizeof(UniformBufferObject) };
        auto descriptorImageInfo = vk::DescriptorImageInfo {
            .sampler = *textureSampler.getSampler(),
            .imageView = *textureImage.getTextureImageView(),
            .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        };

        auto writeDescriptorSets = std::array {
            vk::WriteDescriptorSet {.dstSet = *descriptorSet,
                                    .dstBinding = 0,
                                    .dstArrayElement = 0,
                                    .descriptorCount = 1,
                                    .descriptorType = vk::DescriptorType::eUniformBuffer,
                                    .pBufferInfo = &descriptorBufferInfo},
            vk::WriteDescriptorSet {.dstSet = *descriptorSet,
                                    .dstBinding = 1,
                                    .dstArrayElement = 0,
                                    .descriptorCount = 1,
                                    .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                                    .pImageInfo = &descriptorImageInfo  },
        };
        device.getLogicalDevice().updateDescriptorSets(writeDescriptorSets, nullptr);
    }

    return _descriptorSets;
}

}  // namespace global_ao