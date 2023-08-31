#pragma once
#include <lib/renderer/buffers/image-sampler.hxx>
#include <lib/renderer/buffers/uniform-buffer.txx>
#include <lib/renderer/descriptor-pool.hxx>
#include <lib/renderer/descriptor-set-layouts.hxx>
#include <lib/renderer/device.hxx>
#include <lib/renderer/textures/texture-image.hxx>

namespace global_ao {

// TODO: this needs a complete rethinking, at the moment it assumes one type of uniform buffer object
//       but this is not always the case, we need to be able to handle multiple types of uniform buffer objects
template<typename UniformBufferObjectT>
class DescriptorSets {
  public:
    DescriptorSets(
        const Device& device,
        const DescriptorPool& descriptorPool,
        const DescriptorSetLayouts& descriptorSetLayouts,
        const std::vector<UniformBuffer<UniformBufferObjectT>>& uniformBuffers,
        const Image& image,
        const ImageSampler& imageSampler)
      : descriptorSetLayoutHandles { createDescriptorSetLayoutHandles(descriptorSetLayouts) },
        descriptorSets { createDescriptorSets(device, descriptorPool, uniformBuffers, image, imageSampler) } {};

    DescriptorSets(
        const Device& device,
        const DescriptorPool& descriptorPool,
        const DescriptorSetLayouts& descriptorSetLayouts,
        const std::vector<UniformBuffer<UniformBufferObjectT>>& uniformBuffers)
      : descriptorSetLayoutHandles { createDescriptorSetLayoutHandles(descriptorSetLayouts) },
        descriptorSets { createDescriptorSets(device, descriptorPool, uniformBuffers) } {};

    auto getDescriptorSets() const -> const vk::raii::DescriptorSets& {
        return descriptorSets;
    };

  private:
    static auto createDescriptorSetLayoutHandles(const DescriptorSetLayouts& descriptorSetLayouts)
        -> std::vector<vk::DescriptorSetLayout> {
        auto _descriptorSetLayoutHandles = std::vector<vk::DescriptorSetLayout> {};
        _descriptorSetLayoutHandles.reserve(descriptorSetLayouts.size());

        for (const auto& descriptorSetLayout : descriptorSetLayouts.getDescriptorSetLayouts()) {
            _descriptorSetLayoutHandles.push_back(*descriptorSetLayout);
        }
        return _descriptorSetLayoutHandles;
    };

    auto createDescriptorSets(
        const Device& device,
        const DescriptorPool& descriptorPool,
        const std::vector<UniformBuffer<UniformBufferObjectT>>& uniformBuffers,
        const Image& image,
        const ImageSampler& imageSampler) const -> vk::raii::DescriptorSets {
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
                .sampler = *imageSampler.getSampler(),
                .imageView = *image.getImageView(),
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
    };

    auto createDescriptorSets(
        const Device& device,
        const DescriptorPool& descriptorPool,
        const std::vector<UniformBuffer<UniformBufferObjectT>>& uniformBuffers) -> vk::raii::DescriptorSets {
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

            auto writeDescriptorSets = std::array {
                vk::WriteDescriptorSet {.dstSet = *descriptorSet,
                                        .dstBinding = 0,
                                        .dstArrayElement = 0,
                                        .descriptorCount = 1,
                                        .descriptorType = vk::DescriptorType::eUniformBuffer,
                                        .pBufferInfo = &descriptorBufferInfo},
            };
            device.getLogicalDevice().updateDescriptorSets(writeDescriptorSets, nullptr);
        }

        return _descriptorSets;
    };

    std::vector<vk::DescriptorSetLayout> descriptorSetLayoutHandles;
    vk::raii::DescriptorSets descriptorSets;
};

}  // namespace global_ao
