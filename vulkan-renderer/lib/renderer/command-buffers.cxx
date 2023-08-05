#include <lib/renderer/command-buffers.hxx>

namespace global_ao {
CommandBuffers::CommandBuffers(const size_t size, const Device& device, const CommandPool& commandPool)
  : size { size },
    device { device },
    commandPool { commandPool },
    commandBuffers { createCommandBuffers() } {
}

auto CommandBuffers::getCommandBuffer(const size_t index) const -> const vk::raii::CommandBuffer& {
    return commandBuffers[index];
}

auto CommandBuffers::createCommandBuffers() -> vk::raii::CommandBuffers {
    const auto& _commandPool = commandPool.getCommandPool();
    const auto commandBufferAllocateInfo =
        vk::CommandBufferAllocateInfo { .commandPool = *_commandPool,
                                        .level = vk::CommandBufferLevel::ePrimary,
                                        .commandBufferCount = static_cast<uint32_t>(size) };

    const auto& _device = device.getLogicalDevice();
    return { _device, commandBufferAllocateInfo };
}

auto CommandBuffers::recordCommandBuffer(
    const vk::raii::CommandBuffer& commandBuffer,
    const GraphicsPipeline& pipeline,
    const vk::Framebuffer& frameBuffer,
    const vk::Extent2D& extent) -> void {
    const auto beginInfo = vk::CommandBufferBeginInfo {};

    commandBuffer.begin(beginInfo);

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

    // draw
    commandBuffer.draw(3, 1, 0, 0);

    // end command buffer
    commandBuffer.endRenderPass();
    commandBuffer.end();
    return;
}

}  // namespace global_ao