#include <lib/renderer/ambient-occlusion/pipelines/occlusion-pipeline.hxx>

namespace global_ao {
OcclusionPipeline::OcclusionPipeline(const Device& device,
                                     const UniformBufferObject& ubo,
                                     const std::vector<UniformBuffer<UniformBufferObject>> &uniformBuffers,
                                     const OcclusionPipelineUniformBufferFragmentObject& occlusionUbo,
                                     const std::vector<UniformBuffer<OcclusionPipelineUniformBufferFragmentObject>>& occlusionUniformBuffers,
                                     const DepthResources& depthResources,
                                     const std::unique_ptr<VertexBuffer>& vertexBuffer,
                                     const std::unique_ptr<IndexBuffer>& indexBuffer)
  : device { device },
    ubo { ubo },
    uniformBuffers { uniformBuffers },
    uniformBufferObject { occlusionUbo },
    occlusionUniformBuffers { occlusionUniformBuffers },
    descriptorSetLayouts { createDescriptorSetLayouts(device) },
    depthResources { depthResources },
    imageSampler { device },
    pipeline { createPipeline(vk::Format::eB8G8R8A8Srgb, descriptorSetLayouts, depthResources) },
    occlusionImage {
        device,
        vk::Format::eB8G8R8A8Srgb,
        vk::Extent3D { depthResources.getExtent().width, depthResources.getExtent().height, 1 },
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        vk::ImageAspectFlagBits::eColor
    },
    frameBuffer { device, occlusionImage.getImageView(), depthResources.getExtent(), pipeline },
    commandPool { device },
    commandBuffers { device, 2, commandPool },
    descriptorPool { device, 1 },
    descriptorSets { createDescriptorSets() },
    vertexBuffer { vertexBuffer },
    indexBuffer { indexBuffer } {
}

void OcclusionPipeline::render() {
    transitionDepthImageLayout(device.getGraphicsQueueHandle(), commandBuffers.getCommandBuffer(1));

    device.waitIdle();

    const auto& _frameBuffer = frameBuffer.getFrameBuffer(0);

    // update uniform buffer
    uniformBuffers[0].loadUniformBufferObject(ubo);
    occlusionUniformBuffers[0].loadUniformBufferObject(uniformBufferObject);

    // create command buffer
    const auto& _commandBuffer = commandBuffers.getCommandBuffer(0);
    _commandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    recordCommandBufferForDrawing(_commandBuffer, *_frameBuffer, depthResources.getExtent(), *descriptorSets[0]);

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

auto OcclusionPipeline::createDescriptorSetLayouts(const Device& device) -> DescriptorSetLayouts {
    auto bindings = std::vector<vk::DescriptorSetLayoutBinding> {
        UniformBufferObject::getBindingDescription(),
        ImageSampler::getBindingDescription(1),
        OcclusionPipelineUniformBufferFragmentObject::getBindingDescription()
    };
    return { 1, device, bindings };
}

auto OcclusionPipeline::createPipeline(
    const vk::Format& format,
    const DescriptorSetLayouts& descriptorSetLayouts,
    const DepthResources& depthResources) -> GraphicsPipeline {
    // TODO: move this to the graphics pipeline in a generic way

    const auto attachmentDescriptions = std::vector<vk::AttachmentDescription> {
  // color attachment
        {
         .format = format,
         .samples = vk::SampleCountFlagBits::e1,
         .loadOp = vk::AttachmentLoadOp::eClear,
         .storeOp = vk::AttachmentStoreOp::eStore,
         .stencilLoadOp = vk::AttachmentLoadOp::eDontCare, // no stencil testing for now
 .stencilStoreOp = vk::AttachmentStoreOp::eDontCare, // so don't care
 .initialLayout = vk::ImageLayout::eUndefined,
         .finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        }
  // no depth attachment
    };

    // color attachment reference
    const auto colorAttachmentRef = vk::AttachmentReference {
        .attachment = 0,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    };

    // subpass
    const auto subpass = vk::SubpassDescription {
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef,
        .pDepthStencilAttachment = nullptr,
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
             SHADERS_PATH "/occlusion.vert.bin",
             SHADERS_PATH "/occlusion.frag.bin",
             std::move(renderPass) };
}

auto OcclusionPipeline::recordCommandBufferForDrawing(
    const vk::raii::CommandBuffer& _commandBuffer,
    const vk::Framebuffer& _frameBuffer,
    const vk::Extent2D& extent,
    const vk::DescriptorSet& descriptorSet) -> void {
    _commandBuffer.begin({ /*empty begin info*/ });

    const auto clearValue =
        vk::ClearValue { .color = vk::ClearColorValue { .float32 = std::array { 0.0F, 0.0F, 0.0F, 0.0F } } };

    const auto renderPassInfo = vk::RenderPassBeginInfo {
        .renderPass = *pipeline.getRenderPass(),
        .framebuffer = _frameBuffer,
        .renderArea = vk::Rect2D {.offset = vk::Offset2D { 0, 0 }, .extent = extent},
        .clearValueCount = 1,
        .pClearValues = &clearValue,
    };

    _commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    _commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline.getPipeline());

    // set viewport
    const auto viewport = vk::Viewport { .x = 0.0F,
                                         .y = 0.0F,
                                         .width = static_cast<float>(extent.width),
                                         .height = static_cast<float>(extent.height),
                                         .minDepth = 0.0F,
                                         .maxDepth = 1.0F };
    _commandBuffer.setViewport(0, viewport);


    // set scissor
    const auto scissor = vk::Rect2D {
        .offset = vk::Offset2D {0, 0},
        .extent = extent
    };
    _commandBuffer.setScissor(0, scissor);

    // bind vertex buffer
    _commandBuffer.bindVertexBuffers(0, { *vertexBuffer->getBuffer().getBuffer() }, { 0 });

    // bind index buffer
    _commandBuffer.bindIndexBuffer(*indexBuffer->getBuffer().getBuffer(), 0, vk::IndexType::eUint32);

    // bind descriptor sets
    _commandBuffer
        .bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline.getPipelineLayout(), 0, { descriptorSet }, {});

    // draw
    _commandBuffer.drawIndexed(indexBuffer->getIndexCount(), 1, 0, 0, 0);

    // end command buffer
    _commandBuffer.endRenderPass();
    _commandBuffer.end();
}

auto OcclusionPipeline::transitionDepthImageLayout(
    const vk::raii::Queue& graphicsQueue,
    const vk::raii::CommandBuffer& commandBuffer) -> void {
    recordCommandBufferToTransitionDepthImageLayout(commandBuffer);

    auto submitInfo = vk::SubmitInfo { .commandBufferCount = 1, .pCommandBuffers = &*commandBuffer };
    graphicsQueue.submit(submitInfo);
    graphicsQueue.waitIdle();
}

auto OcclusionPipeline::recordCommandBufferToTransitionDepthImageLayout(const vk::raii::CommandBuffer& commandBuffer)
    -> void {
    const auto beginInfo = vk::CommandBufferBeginInfo { .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
    commandBuffer.begin(beginInfo);

    auto imageBarrier = vk::ImageMemoryBarrier {
        .srcAccessMask = {},
        .dstAccessMask = vk::AccessFlagBits::eShaderRead,
        .oldLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
        .newLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = *depthResources.getImage(),
        .subresourceRange = vk::ImageSubresourceRange { .aspectMask = vk::ImageAspectFlagBits::eDepth,
                          .baseMipLevel = 0,
                          .levelCount = 1,
                          .baseArrayLayer = 0,
                          .layerCount = 1 },
    };

    commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eVertexShader,
        {},
        nullptr,
        nullptr,
        imageBarrier);

    commandBuffer.end();
}

auto OcclusionPipeline::createDescriptorSets() -> vk::raii::DescriptorSets {
    // TODO: this is just a hack? maybe this is the only reasonable way to have different descriptor sets for different
    //       pipelines
    auto _descriptorSetLayoutHandles = std::vector<vk::DescriptorSetLayout> {};
    _descriptorSetLayoutHandles.reserve(descriptorSetLayouts.size());

    for (const auto& descriptorSetLayout : descriptorSetLayouts.getDescriptorSetLayouts()) {
        _descriptorSetLayoutHandles.push_back(*descriptorSetLayout);
    }
    auto descriptorSetAllocateInfo = vk::DescriptorSetAllocateInfo {
        .descriptorPool = *descriptorPool.getDescriptorPool(),
        .descriptorSetCount = static_cast<uint32_t>(descriptorPool.getDescriptorCount()),
        .pSetLayouts = _descriptorSetLayoutHandles.data(),
    };

    auto _descriptorSets = vk::raii::DescriptorSets { device.getLogicalDevice(), descriptorSetAllocateInfo };

    // configure descriptor sets

    for (auto i = 0; i < _descriptorSets.size(); ++i) {
        const auto& descriptorSet = _descriptorSets[i];
        const auto& uniformBuffer = uniformBuffers[i];
        const auto& fragmentSamplesUniformBuffers = occlusionUniformBuffers[i];

        const auto uniformDescriptorBufferInfo =
            vk::DescriptorBufferInfo { .buffer = *uniformBuffer.getBuffer().getBuffer(),
                                       .offset = 0,
                                       .range = sizeof(UniformBufferObject) };
        const auto descriptorImageInfo = vk::DescriptorImageInfo {
            .sampler = *imageSampler.getSampler(),
            .imageView = *depthResources.getImageView(),
            .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        };
        const auto samplesDescriptorBufferInfo =
            vk::DescriptorBufferInfo { .buffer = *fragmentSamplesUniformBuffers.getBuffer().getBuffer(),
                                       .offset = 0,
                                       .range = sizeof(OcclusionPipelineUniformBufferFragmentObject) };

        auto writeDescriptorSets = std::array {
            vk::WriteDescriptorSet {.dstSet = *descriptorSet,
                                    .dstBinding = 0,
                                    .dstArrayElement = 0,
                                    .descriptorCount = 1,
                                    .descriptorType = vk::DescriptorType::eUniformBuffer,
                                    .pBufferInfo = &uniformDescriptorBufferInfo},
            vk::WriteDescriptorSet {.dstSet = *descriptorSet,
                                    .dstBinding = 1,
                                    .dstArrayElement = 0,
                                    .descriptorCount = 1,
                                    .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                                    .pImageInfo = &descriptorImageInfo         },
            vk::WriteDescriptorSet {.dstSet = *descriptorSet,
                                    .dstBinding = 2,
                                    .dstArrayElement = 0,
                                    .descriptorCount = 1,
                                    .descriptorType = vk::DescriptorType::eUniformBuffer,
                                    .pBufferInfo = &samplesDescriptorBufferInfo},
        };
        device.getLogicalDevice().updateDescriptorSets(writeDescriptorSets, nullptr);
    }

    return _descriptorSets;
}

auto OcclusionPipeline::getOcclusionImage() const -> const Image& {
    return occlusionImage;
}

}  // namespace global_ao