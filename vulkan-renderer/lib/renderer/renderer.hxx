#pragma once
#include <lib/renderer/ambient-occlusion/ambient-occlusion.hxx>
#include <lib/renderer/buffer-objects/uniform-buffer-object.hxx>
#include <lib/renderer/buffer-objects/vertex-object.hxx>
#include <lib/renderer/buffers/image-sampler.hxx>
#include <lib/renderer/buffers/index-buffer.hxx>
#include <lib/renderer/buffers/staging-buffer.txx>
#include <lib/renderer/buffers/uniform-buffer.txx>
#include <lib/renderer/buffers/vertex-buffer.hxx>
#include <lib/renderer/command-buffers.hxx>
#include <lib/renderer/command-pool.hxx>
#include <lib/renderer/depth-resources.hxx>
#include <lib/renderer/descriptor-pool.hxx>
#include <lib/renderer/descriptor-set-layouts.hxx>
#include <lib/renderer/descriptor-sets.txx>
#include <lib/renderer/device.hxx>
#include <lib/renderer/dynamic-loader.hxx>
#include <lib/renderer/frame-buffers.hxx>
#include <lib/renderer/graphics-pipeline.hxx>
#include <lib/renderer/instance.hxx>
#include <lib/renderer/surface.hxx>
#include <lib/renderer/swap-chain-provider.hxx>
#include <lib/renderer/sync-objects-handler.hxx>
#include <lib/renderer/textures/texture-image.hxx>
#include <lib/window/window.hxx>

namespace global_ao {

class VulkanRenderer {
  public:
    VulkanRenderer(const Window& window, size_t occlusionSampleSize);

    auto computeOcclusion() -> void;
    auto drawFrame() -> void;
    auto waitIdle() -> void;
    auto loadVerticesWithIndex(const std::vector<VertexObject>& vertices, const std::vector<uint32_t>& indices) -> void;
    auto loadTexture(const std::filesystem::path& texturePath) -> void;
    auto updateUniformBuffer() -> void;
    auto updateDescriptorSets() -> void;

  private:
    auto createDescriptorSetLayouts() -> DescriptorSetLayouts;
    auto createUniformBuffers() -> std::vector<UniformBuffer<UniformBufferObject>>;
    auto createSyncObjects() -> std::vector<SyncObjectsHandler>;
    auto createGraphicsPipeline(const Device& device, const vk::Format& format, const DepthResources& depthResources)
        -> GraphicsPipeline;
    auto recordCommandBufferForDrawing(
        const vk::raii::CommandBuffer& commandBuffer,
        const vk::Framebuffer& frameBuffer,
        const vk::Extent2D& extent,
        const vk::DescriptorSet& descriptorSet) -> void;
    auto recordCommandBufferForLoadingVertices(
        const vk::raii::CommandBuffer& commandBuffer,
        const StagingBuffer<VertexObject>& stagingVertexBuffer,
        const StagingBuffer<uint32_t>& stagingIndexBuffer) -> void;

    auto recreateSwapChain() -> void;

    static constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;
    size_t occlusionSampleSize;
    DynamicLoader dynamicLoader;
    const Window& window;
    Instance instance;
    Surface surface;
    Device device;
    std::unique_ptr<SwapChainHandler> swapChainHandler;
    DescriptorSetLayouts descriptorSetLayouts;
    std::unique_ptr<DepthResources> depthResources;
    GraphicsPipeline pipeline;
    std::unique_ptr<FrameBuffers> frameBuffers;
    CommandPool commandPool;
    std::unique_ptr<TextureImage> textureImage;
    ImageSampler textureSampler;
    std::unique_ptr<VertexBuffer> vertexBuffer;
    std::unique_ptr<IndexBuffer> indexBuffer;
    UniformBufferObject currentUniformBuffer;
    std::vector<UniformBuffer<UniformBufferObject>> uniformBuffers;
    DescriptorPool descriptorPool;
    std::unique_ptr<DescriptorSets<UniformBufferObject>> descriptorSets;
    std::unique_ptr<CommandBuffers> commandBuffers;
    std::vector<SyncObjectsHandler> syncObjectsHandlers;
    std::unique_ptr<AmbientOcclusion> ambientOcclusion;
};

}  // namespace global_ao
