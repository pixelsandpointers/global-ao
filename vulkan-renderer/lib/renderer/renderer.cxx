#include <concepts>
#include <iostream>
#include <lib/renderer/command-buffers.hxx>
#include <lib/renderer/renderer.hxx>

namespace global_ao {
VulkanRenderer::VulkanRenderer(const Window& window)
  : dynamicLoader {},
    window { window },
    instance { window.getVulkanExtensions() },
    surface { instance, window },
    device { instance },
    swapChainHandler { std::make_unique<SwapChainHandler>(device, surface, window) },
    graphicsPipeline { device, *swapChainHandler },
    frameBuffers { std::make_unique<FrameBuffers>(device, *swapChainHandler, graphicsPipeline) },
    commandPool { device },
    commandBuffers { MAX_FRAMES_IN_FLIGHT, device, commandPool },
    syncObjectsHandlers { createSyncObjects() } {
}

auto VulkanRenderer::drawFrame() -> void {
    static auto currentFrame = 0;
    // render image
    // wait for fence
    const auto& _device = device.getLogicalDevice();
    const auto& syncObjects = syncObjectsHandlers[currentFrame].getSyncObjects();
    const auto& inFlightFence = syncObjects.inFlightFence;
    _device.waitForFences({ *inFlightFence }, VK_TRUE, UINT64_MAX);

    // acquire image from swap chain
    const auto& swapChain = swapChainHandler->getSwapChain();
    unsigned int imageIndex;
    try {
        // TODO: this is not ideal, especially with the uninitialized imageIndex above!
        const auto [acquireNextImageResult, _imageIndex] =
            swapChain.acquireNextImage(UINT64_MAX, *syncObjects.imageAvailableSemaphore);
        imageIndex = _imageIndex;
    } catch (vk::OutOfDateKHRError&) {
        recreateSwapChain();
        return;  // we can't continue
    }

    _device.resetFences({ *inFlightFence });

    const auto& frameBuffer = frameBuffers->getFrameBuffer(imageIndex);

    // create command buffer
    const auto& commandBuffer = commandBuffers.getCommandBuffer(currentFrame);
    commandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    commandBuffers.recordCommandBuffer(commandBuffer, graphicsPipeline, *frameBuffer, swapChainHandler->getExtent());

    // submit command buffer
    const auto pipelineStageFlags = vk::PipelineStageFlags { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    const auto& submitInfo = vk::SubmitInfo {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*syncObjects.imageAvailableSemaphore,
        .pWaitDstStageMask = &pipelineStageFlags,
        .commandBufferCount = 1,
        .pCommandBuffers = &*commandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &*syncObjects.renderFinishedSemaphore,
    };
    const auto& graphicsQueue = device.getGraphicsQueueHandle();
    graphicsQueue.submit(submitInfo, *inFlightFence);

    // present image
    const auto presentInfo = vk::PresentInfoKHR { .waitSemaphoreCount = 1,
                                                  .pWaitSemaphores = &*syncObjects.renderFinishedSemaphore,
                                                  .swapchainCount = 1,
                                                  .pSwapchains = &*swapChain,
                                                  .pImageIndices = &imageIndex };
    const auto& presentQueue = device.getPresentQueueHandle();
    try {
        presentQueue.presentKHR(presentInfo);
    } catch (vk::OutOfDateKHRError&) {
        recreateSwapChain();
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

auto VulkanRenderer::waitIdle() -> void {
    const auto& _device = device.getLogicalDevice();
    _device.waitIdle();
}

auto VulkanRenderer::createSyncObjects() -> std::vector<SyncObjectsHandler> {
    std::vector<SyncObjectsHandler> _syncObjectsHandlers;
    _syncObjectsHandlers.reserve(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        _syncObjectsHandlers.emplace_back(device);
    }
    return _syncObjectsHandlers;
}

auto VulkanRenderer::recreateSwapChain() -> void {
    // TODO: should handle minimize case

    waitIdle();

    frameBuffers.reset(nullptr);
    swapChainHandler.reset(nullptr);

    swapChainHandler = std::make_unique<SwapChainHandler>(device, surface, window);   // recreate swap chain
    frameBuffers =
        std::make_unique<FrameBuffers>(device, *swapChainHandler, graphicsPipeline);  // recreate frame buffers
}

}  // namespace global_ao