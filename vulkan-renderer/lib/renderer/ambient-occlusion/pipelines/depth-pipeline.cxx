#include <lib/renderer/ambient-occlusion/pipelines/depth-pipeline.hxx>
#include <span>

namespace global_ao {
DepthPipeline::DepthPipeline(
    const Device& device,
    const vk::Format& format,
    vk::Extent2D extent,
    const DepthPipelineUniformBufferObject& uniformBufferObject,
    const std::vector<UniformBuffer<UniformBufferObject>>& uniformBuffers,
    const std::unique_ptr<IndexBuffer>& indexBuffer,
    const std::unique_ptr<VertexBuffer>& vertexBuffer)
  : device { device },
    uniformBufferObject { uniformBufferObject },
    uniformBuffers { uniformBuffers },
    descriptorSetLayouts { createDescriptorSetLayouts(device) },
    depthResources { device, extent },
    pipeline { createPipeline(format, descriptorSetLayouts, depthResources) },
    frameBuffer { device, depthResources, pipeline },
    commandPool { device },
    commandBuffer { device, 1, commandPool },
    descriptorPool { device, 1 },
    descriptorSets { device, descriptorPool, descriptorSetLayouts, uniformBuffers },
    vertexBuffer { vertexBuffer },
    indexBuffer { indexBuffer } {
}

void DepthPipeline::render() {
    const auto& _device = device.getLogicalDevice();

    const auto& _frameBuffer = frameBuffer.getFrameBuffer(0);

    // update uniform buffer
    uniformBuffers[0].loadUniformBufferObject(uniformBufferObject);

    // create command buffer
    const auto& _commandBuffer = commandBuffer.getCommandBuffer(0);
    _commandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    recordCommandBufferForDrawing(
        _commandBuffer,
        *_frameBuffer,
        depthResources.getExtent(),
        *descriptorSets.getDescriptorSets()[0]);

    // submit command buffer
    const auto pipelineStageFlags = vk::PipelineStageFlags { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    const auto& submitInfo = vk::SubmitInfo {
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,
        .pWaitDstStageMask = &pipelineStageFlags,
        .commandBufferCount = 1,
        .pCommandBuffers = &*_commandBuffer,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = nullptr,
    };
    const auto& graphicsQueue = device.getGraphicsQueueHandle();
    graphicsQueue.submit(submitInfo);
}

auto DepthPipeline::getDepthResources() const -> const DepthResources& {
    return depthResources;
}

auto DepthPipeline::createDescriptorSetLayouts(const Device& device) -> DescriptorSetLayouts {
    auto bindings =
        std::vector<vk::DescriptorSetLayoutBinding> { DepthPipelineUniformBufferObject::getBindingDescription() };
    return { 1, device, bindings };
}

auto DepthPipeline::createPipeline(
    const vk::Format& format,
    const DescriptorSetLayouts& descriptorSetLayouts,
    const DepthResources& depthResources) -> GraphicsPipeline {
    // TODO: move this to the graphics pipeline in a generic way

    const auto attachmentDescriptions = std::vector<vk::AttachmentDescription> {
  // depth attachment
        {
         .format = depthResources.getFormat(),
         .samples = vk::SampleCountFlagBits::e1,
         .loadOp = vk::AttachmentLoadOp::eClear,
         .storeOp = vk::AttachmentStoreOp::eStore,
         .stencilLoadOp = vk::AttachmentLoadOp::eDontCare, // no stencil testing for now
 .stencilStoreOp = vk::AttachmentStoreOp::eDontCare, // so don't care
 .initialLayout = vk::ImageLayout::eUndefined,
         .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
         }
    };

    // depth attachment reference
    const auto depthAttachmentRef = vk::AttachmentReference {
        .attachment = 0,
        .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    // subpass
    const auto subpass = vk::SubpassDescription {
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
        .pDepthStencilAttachment = &depthAttachmentRef,
    };

    // subpass dependency
    const auto subpassDependency = vk::SubpassDependency {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask =
            vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        .dstStageMask =
            vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        .srcAccessMask = vk::AccessFlagBits::eNoneKHR,
        .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
    };

    // actual render pass
    const auto renderPassCreateInfo = vk::RenderPassCreateInfo {
        .attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size()),
        .pAttachments = attachmentDescriptions.data(),
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &subpassDependency,
    };

    auto renderPass = vk::raii::RenderPass { device.getLogicalDevice(), renderPassCreateInfo };

    return { device,
             format,
             descriptorSetLayouts,
             SHADERS_PATH "/depth.vert.bin",
             SHADERS_PATH "/depth.frag.bin",
             std::move(renderPass) };
}

auto DepthPipeline::recordCommandBufferForDrawing(
    const vk::raii::CommandBuffer& commandBuffer,
    const vk::Framebuffer& frameBuffer,
    const vk::Extent2D& extent,
    const vk::DescriptorSet& descriptorSet) -> void {
    commandBuffer.begin({ /*empty begin info*/ });

    const auto clearValues =
        std::array { vk::ClearValue { .depthStencil = vk::ClearDepthStencilValue { 1.0F, 0 } } };  // 1 is the max depth

    const auto renderPassInfo = vk::RenderPassBeginInfo {
        .renderPass = *pipeline.getRenderPass(),
        .framebuffer = frameBuffer,
        .renderArea = vk::Rect2D {.offset = vk::Offset2D { 0, 0 }, .extent = extent},
        .clearValueCount = static_cast<uint32_t>(clearValues.size()),
        .pClearValues = clearValues.data(),
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
}

}  // namespace global_ao