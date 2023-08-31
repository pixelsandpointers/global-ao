#include <lib/renderer/ambient-occlusion/pipelines/accumulation-pipeline.hxx>

namespace global_ao {

AccumulationPipeline::AccumulationPipeline(
    const Device& device,
    const std::unique_ptr<VertexBuffer>& vertexBuffer,
    const std::unique_ptr<IndexBuffer>& indexBuffer,
    const Image& occlusionImage)
  : device { device },
    vertexBuffer { vertexBuffer },
    indexBuffer { indexBuffer },
    occlusionImage { occlusionImage },
    imageSampler { device },
    descriptorSetLayouts { createDescriptorSetLayouts(device) },
    pipeline { createPipeline(occlusionImage.getFormat()) },
    accumulationImages { createAccumulationImages(device) },
    frameBuffers { device, std::span(accumulationImages), pipeline },
    commandPool { device },
    commandBuffers { device, 2, commandPool },
    descriptorPool { device, 2 },
    descriptorSets { createDescriptorSets() } {
}

auto AccumulationPipeline::render(size_t index) -> void {
    transitionAccumulationImageLayout(
        (index + 1) % 2,
        commandBuffers.getCommandBuffer(1),
        device.getGraphicsQueueHandle());

    device.waitIdle();


    const auto& _frameBuffer = frameBuffers.getFrameBuffer(index);
    const auto extent = occlusionImage.getExtent();

    // create command buffer
    const auto& _commandBuffer = commandBuffers.getCommandBuffer(0);
    _commandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    recordCommandBufferForDrawing(
        _commandBuffer,
        *_frameBuffer,
        vk::Extent2D { extent.width, extent.height },
        *descriptorSets[index]);

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

auto AccumulationPipeline::getAccumulationImage() const -> const Image& {
    return accumulationImages[0];  // TODO: little bit of a hack here but having two accumulation images is a short
                                   //       coming of this method in the first place which should be addressed first
}

auto AccumulationPipeline::createDescriptorSetLayouts(const Device& device) -> DescriptorSetLayouts {
    auto bindings = std::vector<vk::DescriptorSetLayoutBinding> {
        ImageSampler::getBindingDescription(0),
        ImageSampler::getBindingDescription(1),
    };
    return { 2, device, bindings };
}

auto AccumulationPipeline::createPipeline(const vk::Format& format) -> GraphicsPipeline {
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
             SHADERS_PATH "/accumulation.vert.bin",
             SHADERS_PATH "/accumulation.frag.bin",
             std::move(renderPass) };
}

auto AccumulationPipeline::createDescriptorSets() -> vk::raii::DescriptorSets {
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

    // case 1: use second accumulation map as current map
    const auto& descriptorSet1 = _descriptorSets[0];
    const auto currentMapDescriptorImageInfo1 = vk::DescriptorImageInfo {
        .sampler = *imageSampler.getSampler(),
        .imageView = *accumulationImages[1].getImageView(),
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
    };
    const auto newMapDescriptorImageInfo1 = vk::DescriptorImageInfo {
        .sampler = *imageSampler.getSampler(),
        .imageView = *occlusionImage.getImageView(),
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
    };

    auto writeDescriptorSets1 = std::array {
        vk::WriteDescriptorSet {.dstSet = *descriptorSet1,
                                .dstBinding = 0,
                                .dstArrayElement = 0,
                                .descriptorCount = 1,
                                .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                                .pImageInfo = &currentMapDescriptorImageInfo1},
        vk::WriteDescriptorSet {.dstSet = *descriptorSet1,
                                .dstBinding = 1,
                                .dstArrayElement = 0,
                                .descriptorCount = 1,
                                .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                                .pImageInfo = &newMapDescriptorImageInfo1    },
    };
    device.getLogicalDevice().updateDescriptorSets(writeDescriptorSets1, nullptr);

    // case 2: use first accumulation map as current map
    const auto& descriptorSet2 = _descriptorSets[1];
    const auto currentMapDescriptorImageInfo2 = vk::DescriptorImageInfo {
        .sampler = *imageSampler.getSampler(),
        .imageView = *accumulationImages[0].getImageView(),
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
    };
    const auto newMapDescriptorImageInfo2 = vk::DescriptorImageInfo {
        .sampler = *imageSampler.getSampler(),
        .imageView = *occlusionImage.getImageView(),
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
    };

    auto writeDescriptorSets2 = std::array {
        vk::WriteDescriptorSet {.dstSet = *descriptorSet2,
                                .dstBinding = 0,
                                .dstArrayElement = 0,
                                .descriptorCount = 1,
                                .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                                .pImageInfo = &currentMapDescriptorImageInfo2},
        vk::WriteDescriptorSet {.dstSet = *descriptorSet2,
                                .dstBinding = 1,
                                .dstArrayElement = 0,
                                .descriptorCount = 1,
                                .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                                .pImageInfo = &newMapDescriptorImageInfo2    },
    };
    device.getLogicalDevice().updateDescriptorSets(writeDescriptorSets2, nullptr);
    return _descriptorSets;
}

auto AccumulationPipeline::createAccumulationImages(const Device& device) -> std::array<Image, 2> {
    const auto extent = occlusionImage.getExtent();
    return {
        Image {device,
               vk::Format::eB8G8R8A8Srgb,
               vk::Extent3D { extent.width, extent.height, 1 },
               vk::ImageTiling::eOptimal,
               vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
               vk::MemoryPropertyFlagBits::eDeviceLocal,
               vk::ImageAspectFlagBits::eColor},
        Image {device,
               vk::Format::eB8G8R8A8Srgb,
               vk::Extent3D { extent.width, extent.height, 1 },
               vk::ImageTiling::eOptimal,
               vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
               vk::MemoryPropertyFlagBits::eDeviceLocal,
               vk::ImageAspectFlagBits::eColor},
    };
}

auto AccumulationPipeline::recordCommandBufferForDrawing(
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

auto AccumulationPipeline::transitionAccumulationImageLayout(
    const size_t index,
    const vk::raii::CommandBuffer& commandBuffer,
    const vk::raii::Queue& graphicsQueue) -> void {
    recordCommandBufferToTransitionAccumulationImageLayout(index, commandBuffer);

    auto submitInfo = vk::SubmitInfo { .commandBufferCount = 1, .pCommandBuffers = &*commandBuffer };
    graphicsQueue.submit(submitInfo);
    graphicsQueue.waitIdle();
}

auto AccumulationPipeline::recordCommandBufferToTransitionAccumulationImageLayout(
    const size_t index,
    const vk::raii::CommandBuffer& commandBuffer) -> void {
    const auto beginInfo = vk::CommandBufferBeginInfo { .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
    commandBuffer.begin(beginInfo);

    auto imageBarrier = vk::ImageMemoryBarrier {
        .srcAccessMask = {},
        .dstAccessMask = vk::AccessFlagBits::eShaderRead,
        .oldLayout = vk::ImageLayout::eUndefined,
        .newLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = *accumulationImages[index].getImage(),
        .subresourceRange = vk::ImageSubresourceRange { .aspectMask = vk::ImageAspectFlagBits::eColor,
                          .baseMipLevel = 0,
                          .levelCount = 1,
                          .baseArrayLayer = 0,
                          .layerCount = 1 },
    };

    commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eFragmentShader,
        {},
        nullptr,
        nullptr,
        imageBarrier);

    commandBuffer.end();
}

}  // namespace global_ao