#pragma once
#include <lib/renderer/buffers/staging-buffer.hxx>
#include <lib/renderer/buffers/vertex-buffer.hxx>
#include <lib/renderer/command-buffers.hxx>
#include <lib/renderer/command-pool.hxx>
#include <lib/renderer/device.hxx>
#include <lib/renderer/dynamic-loader.hxx>
#include <lib/renderer/frame-buffer.hxx>
#include <lib/renderer/graphics-pipeline.hxx>
#include <lib/renderer/instance.hxx>
#include <lib/renderer/surface.hxx>
#include <lib/renderer/swap-chain-provider.hxx>
#include <lib/renderer/sync-objects-handler.hxx>
#include <lib/renderer/vertex.hxx>
#include <lib/window/window.hxx>

namespace global_ao {

class VulkanRenderer {
  public:
    explicit VulkanRenderer(const Window& window);

    auto drawFrame() -> void;
    auto waitIdle() -> void;
    auto loadVertices(const std::vector<Vertex>& vertices) -> void;

  private:
    auto createSyncObjects() -> std::vector<SyncObjectsHandler>;
    auto recordCommandBufferForDrawing(
        const vk::raii::CommandBuffer& commandBuffer,
        const vk::Framebuffer& frameBuffer,
        const vk::Extent2D& extent) -> void;
    auto recordCommandBufferForLoadingVertices(
        const vk::raii::CommandBuffer& commandBuffer,
        const StagingBuffer& stagingBuffer) -> void;

    auto recreateSwapChain() -> void;

    static constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;
    DynamicLoader dynamicLoader;
    const Window& window;
    Instance instance;
    Surface surface;
    Device device;
    std::unique_ptr<SwapChainHandler> swapChainHandler;
    GraphicsPipeline pipeline;
    std::unique_ptr<FrameBuffers> frameBuffers;
    CommandPool commandPool;
    std::unique_ptr<VertexBuffer> vertexBuffer;
    CommandBuffers commandBuffers;
    std::vector<SyncObjectsHandler> syncObjectsHandlers;
};

}  // namespace global_ao
