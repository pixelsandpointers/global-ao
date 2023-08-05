#pragma once
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
#include <lib/window/window.hxx>

namespace global_ao {

class VulkanRenderer {
  public:
    explicit VulkanRenderer(const Window& window);

    auto drawFrame() -> void;
    auto waitIdle() -> void;

  private:
    auto createSyncObjects() -> std::vector<SyncObjectsHandler>;
    auto recreateSwapChain() -> void;

    static constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;
    DynamicLoader dynamicLoader;
    const Window& window;
    Instance instance;
    Surface surface;
    Device device;
    std::unique_ptr<SwapChainHandler> swapChainHandler;
    GraphicsPipeline graphicsPipeline;
    std::unique_ptr<FrameBuffers> frameBuffers;
    CommandPool commandPool;
    CommandBuffers commandBuffers;
    std::vector<SyncObjectsHandler> syncObjectsHandlers;
};

}  // namespace global_ao
