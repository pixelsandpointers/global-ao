#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define STB_IMAGE_IMPLEMENTATION
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <lib/renderer/buffers/buffer.hxx>
#include <lib/renderer/buffers/staging-buffer.txx>
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
    descriptorSetLayout { device },
    pipeline { device, *swapChainHandler, descriptorSetLayout },
    frameBuffers { std::make_unique<FrameBuffers>(device, *swapChainHandler, pipeline) },
    commandPool { device },
    textureImage {},
    vertexBuffer {},
    indexBuffer {},
    uniformBuffers { createUniformBuffers() },
    descriptorPool { device, MAX_FRAMES_IN_FLIGHT },
    descriptorSets { device, descriptorPool, descriptorSetLayout, uniformBuffers },
    commandBuffers { commandPool.createCommandBuffers(MAX_FRAMES_IN_FLIGHT) },
    syncObjectsHandlers { createSyncObjects() } {
}

auto VulkanRenderer::drawFrame() -> void {
    static auto currentFrame = 0;
    // render image
    // wait for fence
    const auto& _device = device.getLogicalDevice();
    const auto& syncObjects = syncObjectsHandlers[currentFrame].getSyncObjects();
    const auto& inFlightFence = syncObjects.inFlightFence;
    auto _ = _device.waitForFences(
        { *inFlightFence },
        VK_TRUE,
        UINT64_MAX);  // to avoid deadlock it's initialized to signaled

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

    // reset fence because we are going to use it again
    _device.resetFences({ *inFlightFence });

    const auto& frameBuffer = frameBuffers->getFrameBuffer(imageIndex);

    // update uniform buffer
    uniformBuffers[currentFrame].loadUniformBuffer(currentUniformBuffer);

    // create command buffer
    const auto& commandBuffer = commandBuffers.getCommandBuffer(currentFrame);
    commandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    recordCommandBufferForDrawing(
        commandBuffer,
        *frameBuffer,
        swapChainHandler->getExtent(),
        *descriptorSets.getDescriptorSets()[currentFrame]);

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

auto VulkanRenderer::loadVerticesWithIndex(
    const std::vector<VertexObject>& vertices,
    const std::vector<uint32_t>& indices) -> void {
    // TODO: it's recommended to use the same buffer for the vertices and indices and use offsets to access them
    //       see: https://developer.nvidia.com/vulkan-memory-management

    // load data into a staging buffer which is host visible (cpu can access it)
    // then copy the data from the staging buffer to the vertex buffer which is device local (on the gpu and not
    // accessible by the cpu directly)
    // this is because it's faster for the gpu to read from device local memory
    auto stagingVertexBuffer = StagingBuffer { device, vertices };
    vertexBuffer = std::make_unique<VertexBuffer>(device, stagingVertexBuffer.getSize(), vertices.size());

    auto stagingIndexBuffer = StagingBuffer { device, indices };
    indexBuffer = std::make_unique<IndexBuffer>(device, stagingIndexBuffer.getSize(), indices.size());

    // to actually get the data to the gpu local vertexBuffer, we need to put a query into the graphics queue (which
    // implicitly also is a transfer queue)

    // first we need to create a command buffer
    // we are even going to create a new command pool for possible optimizations
    const auto _commandPool = CommandPool { device, vk::CommandPoolCreateFlagBits::eTransient };
    const auto commandBuffers = _commandPool.createCommandBuffers(1);
    const auto& commandBuffer = commandBuffers.getCommandBuffer(0);
    recordCommandBufferForLoadingVertices(commandBuffer, stagingVertexBuffer, stagingIndexBuffer);

    // then we need to submit it to the queue
    const auto& submitInfo = vk::SubmitInfo { .commandBufferCount = 1, .pCommandBuffers = &*commandBuffer };

    const auto& graphicsQueue = device.getGraphicsQueueHandle();
    graphicsQueue.submit(submitInfo);
    waitIdle();
}

auto VulkanRenderer::loadTexture(const std::filesystem::path& texturePath) -> void {
    const auto& graphicsQueue = device.getGraphicsQueueHandle();
    const auto _commandPool = CommandPool { device, vk::CommandPoolCreateFlagBits::eTransient };
    textureImage = std::make_unique<TextureImage>(device, texturePath, graphicsQueue, _commandPool);
}

auto VulkanRenderer::updateUniformBuffer() -> void {
    static auto startTime = std::chrono::high_resolution_clock::now();

    const auto currentTime = std::chrono::high_resolution_clock::now();
    const auto time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    auto ubo = UniformBufferObject {};
    ubo.model = glm::rotate(glm::mat4(1.0F), time * glm::radians(90.0F), glm::vec3(0.0F, 0.0F, 1.0F));
    ubo.view = glm::lookAt(glm::vec3(2.0F, 2.0F, 2.0F), glm::vec3(0.0F, 0.0F, 0.0F), glm::vec3(0.0F, 0.0F, 1.0F));

    const auto extent = swapChainHandler->getExtent();
    ubo.proj = glm::perspective(
        glm::radians(45.0F),
        static_cast<float>(extent.width) / static_cast<float>(extent.height),
        0.1F,
        10.0F);

    ubo.proj[1][1] *= -1;  // invert y coordinate
    currentUniformBuffer = ubo;
}

auto VulkanRenderer::createUniformBuffers() -> std::vector<UniformBuffer> {
    std::vector<UniformBuffer> _uniformBuffers;
    _uniformBuffers.reserve(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        _uniformBuffers.emplace_back(device);
    }
    return _uniformBuffers;
}

auto VulkanRenderer::createSyncObjects() -> std::vector<SyncObjectsHandler> {
    std::vector<SyncObjectsHandler> _syncObjectsHandlers;
    _syncObjectsHandlers.reserve(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        _syncObjectsHandlers.emplace_back(device);
    }
    return _syncObjectsHandlers;
}

auto VulkanRenderer::recordCommandBufferForDrawing(
    const vk::raii::CommandBuffer& commandBuffer,
    const vk::Framebuffer& frameBuffer,
    const vk::Extent2D& extent,
    const vk::DescriptorSet& descriptorSet) -> void {
    commandBuffer.begin({ /*empty begin info*/ });

    const auto clearValue =
        vk::ClearValue { .color = vk::ClearColorValue { .float32 = std::array { 0.0F, 0.0F, 0.0F, 0.0F } } };

    const auto renderPassInfo = vk::RenderPassBeginInfo {
        .renderPass = *pipeline.getRenderPass(),
        .framebuffer = frameBuffer,
        .renderArea = vk::Rect2D {.offset = vk::Offset2D { 0, 0 }, .extent = extent},
        .clearValueCount = 1,
        .pClearValues = &clearValue
    };

    commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline.getPipeline());

    // set viewport
    const auto viewport = vk::Viewport { .x = 0.0F,
                                         .y = 0.0F,
                                         .width = static_cast<float>(extent.width),
                                         .height = static_cast<float>(extent.height),
                                         .minDepth = 0.0F,
                                         .maxDepth = 1.0F };
    commandBuffer.setViewport(0, viewport);


    // set scissor
    const auto scissor = vk::Rect2D {
        .offset = vk::Offset2D {0, 0},
        .extent = extent
    };
    commandBuffer.setScissor(0, scissor);

    // bind vertex buffer
    commandBuffer.bindVertexBuffers(0, { *vertexBuffer->getBuffer().getBuffer() }, { 0 });

    // bind index buffer
    commandBuffer.bindIndexBuffer(*indexBuffer->getBuffer().getBuffer(), 0, vk::IndexType::eUint32);

    // bind descriptor sets
    commandBuffer
        .bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline.getPipelineLayout(), 0, { descriptorSet }, {});

    // draw
    commandBuffer.drawIndexed(indexBuffer->getIndexCount(), 1, 0, 0, 0);

    // end command buffer
    commandBuffer.endRenderPass();
    commandBuffer.end();
    return;
}

auto VulkanRenderer::recreateSwapChain() -> void {
    // TODO: should handle minimize case

    waitIdle();

    frameBuffers.reset(nullptr);
    swapChainHandler.reset(nullptr);

    swapChainHandler = std::make_unique<SwapChainHandler>(device, surface, window);      // recreate swap chain
    frameBuffers = std::make_unique<FrameBuffers>(device, *swapChainHandler, pipeline);  // recreate frame buffers
}

auto VulkanRenderer::recordCommandBufferForLoadingVertices(
    const vk::raii::CommandBuffer& commandBuffer,
    const StagingBuffer<VertexObject>& stagingVertexBuffer,
    const StagingBuffer<uint32_t>& stagingIndexBuffer) -> void {
    // load vertices and indices into the vertex and index buffers
    // tell the driver that we only need this command buffer once (contrary to the draw command buffer)
    const auto beginInfo = vk::CommandBufferBeginInfo { .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
    commandBuffer.begin(beginInfo);

    // vertex buffer
    const auto vertexCopyRegion = vk::BufferCopy { .size = stagingVertexBuffer.getSize() };
    commandBuffer.copyBuffer(
        *stagingVertexBuffer.getBuffer().getBuffer(),
        *vertexBuffer->getBuffer().getBuffer(),
        vertexCopyRegion);

    // index buffer
    const auto indexCopyRegion = vk::BufferCopy { .size = stagingIndexBuffer.getSize() };
    commandBuffer.copyBuffer(
        *stagingIndexBuffer.getBuffer().getBuffer(),
        *indexBuffer->getBuffer().getBuffer(),
        indexCopyRegion);

    commandBuffer.end();
}

}  // namespace global_ao