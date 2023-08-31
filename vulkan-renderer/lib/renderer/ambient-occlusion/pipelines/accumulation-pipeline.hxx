#pragma once

#include <lib/renderer/buffers/index-buffer.hxx>
#include <lib/renderer/buffers/uniform-buffer.txx>
#include <lib/renderer/buffers/vertex-buffer.hxx>
#include <lib/renderer/command-buffers.hxx>
#include <lib/renderer/command-pool.hxx>
#include <lib/renderer/descriptor-pool.hxx>
#include <lib/renderer/descriptor-set-layouts.hxx>
#include <lib/renderer/descriptor-sets.txx>
#include <lib/renderer/device.hxx>
#include <lib/renderer/frame-buffers.hxx>
#include <lib/renderer/graphics-pipeline.hxx>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class AccumulationPipeline {
  public:
    AccumulationPipeline(
        const Device& device,
        const std::unique_ptr<VertexBuffer>& vertexBuffer,
        const std::unique_ptr<IndexBuffer>& indexBuffer,
        const Image& occlusionImage);

    auto render(size_t index) -> void;

    auto getAccumulationImage() const -> const Image&;

  private:
    auto createDescriptorSetLayouts(const Device& device) -> DescriptorSetLayouts;
    auto createPipeline(const vk::Format& format) -> GraphicsPipeline;
    auto createDescriptorSets() -> vk::raii::DescriptorSets;
    auto transitionAccumulationImageLayout(
        const size_t index,
        const vk::raii::CommandBuffer& commandBuffer,
        const vk::raii::Queue& graphicsQueue) -> void;
    auto createAccumulationImages(const Device& device) -> std::array<Image, 2> ;
    auto recordCommandBufferForDrawing(
        const vk::raii::CommandBuffer& _commandBuffer,
        const vk::Framebuffer& _frameBuffer,
        const vk::Extent2D& extent,
        const vk::DescriptorSet& descriptorSet) -> void;
    auto recordCommandBufferToTransitionAccumulationImageLayout(
        const size_t index,
        const vk::raii::CommandBuffer& commandBuffer) -> void;

    const Device& device;
    const std::unique_ptr<VertexBuffer>& vertexBuffer;
    const std::unique_ptr<IndexBuffer>& indexBuffer;
    const Image& occlusionImage;
    const ImageSampler imageSampler;
    const DescriptorSetLayouts descriptorSetLayouts;
    const GraphicsPipeline pipeline;
    const std::array<Image, 2> accumulationImages;
    const FrameBuffers frameBuffers;
    const CommandPool commandPool;
    const CommandBuffers commandBuffers;
    const DescriptorPool descriptorPool;
    vk::raii::DescriptorSets
        descriptorSets;  // TODO: this is just hacked in here, it should ideally use the DescriptorSets class
};

}  // namespace global_ao