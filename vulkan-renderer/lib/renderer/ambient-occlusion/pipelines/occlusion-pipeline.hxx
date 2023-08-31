#pragma once

#include <lib/renderer/buffer-objects/occlusion-pipeline-uniform-buffer-object.hxx>
#include <lib/renderer/buffers/index-buffer.hxx>
#include <lib/renderer/buffers/uniform-buffer.txx>
#include <lib/renderer/buffers/vertex-buffer.hxx>
#include <lib/renderer/command-buffers.hxx>
#include <lib/renderer/command-pool.hxx>
#include <lib/renderer/depth-resources.hxx>
#include <lib/renderer/descriptor-pool.hxx>
#include <lib/renderer/descriptor-set-layouts.hxx>
#include <lib/renderer/descriptor-sets.txx>
#include <lib/renderer/device.hxx>
#include <lib/renderer/frame-buffers.hxx>
#include <lib/renderer/graphics-pipeline.hxx>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class OcclusionPipeline {
  public:
    OcclusionPipeline(
        const Device& device,
        const UniformBufferObject& ubo,
        const std::vector<UniformBuffer<UniformBufferObject>>& uniformBuffers,
        const OcclusionPipelineUniformBufferFragmentObject& occlusionUbo,
        const std::vector<UniformBuffer<OcclusionPipelineUniformBufferFragmentObject>>& occlusionUniformBuffers,
        const DepthResources& depthResources,
        const std::unique_ptr<VertexBuffer>& vertexBuffer,
        const std::unique_ptr<IndexBuffer>& indexBuffer);

    void render();

    auto getOcclusionImage() const -> const Image&;

  private:
    auto createDescriptorSetLayouts(const Device& device) -> DescriptorSetLayouts;
    auto createPipeline(
        const vk::Format& format,
        const DescriptorSetLayouts& descriptorSetLayouts,
        const DepthResources& depthResources) -> GraphicsPipeline;
    auto createDescriptorSets() -> vk::raii::DescriptorSets;
    auto transitionDepthImageLayout(const vk::raii::Queue& graphicsQueue, const vk::raii::CommandBuffer& commandBuffer)
        -> void;
    auto recordCommandBufferForDrawing(
        const vk::raii::CommandBuffer& _commandBuffer,
        const vk::Framebuffer& _frameBuffer,
        const vk::Extent2D& extent,
        const vk::DescriptorSet& descriptorSet) -> void;
    auto recordCommandBufferToTransitionDepthImageLayout(const vk::raii::CommandBuffer& commandBuffer) -> void;

    const Device& device;
    const UniformBufferObject& ubo;
    const std::vector<UniformBuffer<UniformBufferObject>>& uniformBuffers;
    const OcclusionPipelineUniformBufferFragmentObject& uniformBufferObject;
    const std::vector<UniformBuffer<OcclusionPipelineUniformBufferFragmentObject>>& occlusionUniformBuffers;
    const DescriptorSetLayouts descriptorSetLayouts;
    const DepthResources& depthResources;
    const ImageSampler imageSampler;
    const GraphicsPipeline pipeline;
    const Image occlusionImage;
    const FrameBuffers frameBuffer;
    const CommandPool commandPool;
    const CommandBuffers commandBuffers;
    const DescriptorPool descriptorPool;
    vk::raii::DescriptorSets
        descriptorSets;  // TODO: this is just hacked in here, it should ideally use the DescriptorSets class
    const std::unique_ptr<VertexBuffer>& vertexBuffer;
    const std::unique_ptr<IndexBuffer>& indexBuffer;
};

}  // namespace global_ao
