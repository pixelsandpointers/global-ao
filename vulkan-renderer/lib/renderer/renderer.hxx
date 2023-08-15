#pragma once
#include <lib/renderer/buffers/index-buffer.hxx>
#include <lib/renderer/buffers/staging-buffer.hxx>
#include <lib/renderer/buffers/uniform-buffer-object.hxx>
#include <lib/renderer/buffers/uniform-buffer.hxx>
#include <lib/renderer/buffers/vertex-buffer.hxx>
#include <lib/renderer/buffers/vertex-object.hxx>
#include <lib/renderer/command-buffers.hxx>
#include <lib/renderer/command-pool.hxx>
#include <lib/renderer/descriptor-pool.hxx>
#include <lib/renderer/descriptor-set-layout.hxx>
#include <lib/renderer/descriptor-sets.hxx>
#include <lib/renderer/device.hxx>
#include <lib/renderer/dynamic-loader.hxx>
#include <lib/renderer/frame-buffer.hxx>
#include <lib/renderer/graphics-pipeline.hxx>
#include <lib/renderer/instance.hxx>
#include <lib/renderer/surface.hxx>
#include <lib/renderer/swap-chain-provider.hxx>
#include <lib/renderer/sync-objects-handler.hxx>
#include <lib/window/window.hxx>

namespace global_ao {

class VulkanRenderer {
  public:
    explicit VulkanRenderer(const Window& window);

    auto drawFrame() -> void;
    auto waitIdle() -> void;
    auto loadVerticesWithIndex(const std::vector<VertexObject>& vertices, const std::vector<uint32_t>& indices) -> void;
    auto updateUniformBuffer() -> void;

  private:
    auto createUniformBuffers() -> std::vector<UniformBuffer>;
    auto createSyncObjects() -> std::vector<SyncObjectsHandler>;
    auto recordCommandBufferForDrawing(
        const vk::raii::CommandBuffer& commandBuffer,
        const vk::Framebuffer& frameBuffer,
        const vk::Extent2D& extent,
        const vk::DescriptorSet& descriptorSet) -> void;
    auto recordCommandBufferForLoadingVertices(
        const vk::raii::CommandBuffer& commandBuffer,
        const StagingBuffer& stagingVertexBuffer,
        const StagingBuffer& stagingIndexBuffer) -> void;

    auto recreateSwapChain() -> void;

    static constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;
    DynamicLoader dynamicLoader;
    const Window& window;
    Instance instance;
    Surface surface;
    Device device;
    std::unique_ptr<SwapChainHandler> swapChainHandler;
    DescriptorSetLayout descriptorSetLayout;
    GraphicsPipeline pipeline;
    std::unique_ptr<FrameBuffers> frameBuffers;
    CommandPool commandPool;
    std::unique_ptr<VertexBuffer> vertexBuffer;
    std::unique_ptr<IndexBuffer> indexBuffer;
    UniformBufferObject currentUniformBuffer;
    std::vector<UniformBuffer> uniformBuffers;
    DescriptorPool descriptorPool;
    DescriptorSets descriptorSets;
    CommandBuffers commandBuffers;
    std::vector<SyncObjectsHandler> syncObjectsHandlers;
};

}  // namespace global_ao
