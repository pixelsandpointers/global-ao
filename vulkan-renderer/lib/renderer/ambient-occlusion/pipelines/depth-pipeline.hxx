#pragma once
#include <lib/renderer/buffer-objects/depth-pipeline-uniform-buffer-object.hxx>
#include <lib/renderer/buffer-objects/uniform-buffer-object.hxx>
#include <lib/renderer/buffers/index-buffer.hxx>
#include <lib/renderer/buffers/uniform-buffer.txx>
#include <lib/renderer/buffers/vertex-buffer.hxx>
#include <lib/renderer/command-buffers.hxx>
#include <lib/renderer/command-pool.hxx>
#include <lib/renderer/depth-resources.hxx>
#include <lib/renderer/descriptor-sets.txx>
#include <lib/renderer/device.hxx>
#include <lib/renderer/frame-buffers.hxx>
#include <lib/renderer/graphics-pipeline.hxx>
#include <lib/renderer/sync-objects-handler.hxx>

namespace global_ao {

class DepthPipeline {
  public:
    DepthPipeline(
        const Device& device,
        const vk::Format& format,
        vk::Extent2D extent,
        const DepthPipelineUniformBufferObject& uniformBufferObject,
        const std::vector<UniformBuffer<UniformBufferObject>>& uniformBuffers,
        const std::unique_ptr<IndexBuffer>& indexBuffer,
        const std::unique_ptr<VertexBuffer>& vertexBuffer);

    auto render() -> void;

    auto getDepthResources() const -> const DepthResources&;

  private:
    auto createDescriptorSetLayouts(const Device& device) -> DescriptorSetLayouts;
    auto createPipeline(
        const vk::Format& format,
        const DescriptorSetLayouts& descriptorSetLayouts,
        const DepthResources& depthResources) -> GraphicsPipeline;
    auto recordCommandBufferForDrawing(
        const vk::raii::CommandBuffer& commandBuffer,
        const vk::Framebuffer& frameBuffer,
        const vk::Extent2D& extent,
        const vk::DescriptorSet& descriptorSet) -> void;

    const Device& device;
    const UniformBufferObject& uniformBufferObject;
    const std::vector<UniformBuffer<UniformBufferObject>>& uniformBuffers;
    const DescriptorSetLayouts descriptorSetLayouts;
    const DepthResources depthResources;
    const GraphicsPipeline pipeline;
    const FrameBuffers frameBuffer;
    const CommandPool commandPool;
    const CommandBuffers commandBuffer;
    const DescriptorPool descriptorPool;
    const DescriptorSets<UniformBufferObject> descriptorSets;
    const std::unique_ptr<VertexBuffer>& vertexBuffer;
    const std::unique_ptr<IndexBuffer>& indexBuffer;
};

}  // namespace global_ao
