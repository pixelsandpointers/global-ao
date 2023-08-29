#include <lib/renderer/buffers/vertex-object.hxx>
#include <lib/renderer/graphics-pipeline.hxx>

namespace global_ao {
GraphicsPipeline::GraphicsPipeline(
    const Device& device,
    const SwapChainHandler& imageViewProvider,
    const DescriptorSetLayout& descriptorSetLayout,
    const DepthResources& depthResources)
  : device { device },
    imageViewProvider { imageViewProvider },
    descriptorSetLayout { descriptorSetLayout },
    vertexShader { shader_vert_PATH, device },
    fragmentShader { shader_frag_PATH, device },
    attachments { createAttachments(depthResources) },
    renderPass { createRenderPass() },
    pipelineLayout { createPipelineLayout() },
    pipeline { createPipeline() } {
}

auto GraphicsPipeline::getPipeline() const -> const vk::raii::Pipeline& {
    return pipeline;
}

auto GraphicsPipeline::getRenderPass() const -> const vk::raii::RenderPass& {
    return renderPass;
}

auto GraphicsPipeline::getPipelineLayout() const -> const vk::raii::PipelineLayout& {
    return pipelineLayout;
}

auto GraphicsPipeline::createAttachments(const DepthResources& depthResources) const
    -> std::array<vk::AttachmentDescription, 2> {
    // color attachment
    const auto surfaceFormat = imageViewProvider.getSurfaceFormat();
    const auto colorAttachment = vk::AttachmentDescription {
        .format = surfaceFormat.format,
        .samples = vk::SampleCountFlagBits::e1,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,    // no stencil testing for now
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,  // so don't care
        .initialLayout = vk::ImageLayout::eUndefined,
        .finalLayout = vk::ImageLayout::ePresentSrcKHR,
    };

    // depth attachment
    const auto depthAttachment = vk::AttachmentDescription {
        .format = depthResources.getFormat(),
        .samples = vk::SampleCountFlagBits::e1,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,    // no stencil testing for now
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,  // so don't care
        .initialLayout = vk::ImageLayout::eUndefined,
        .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };
    return { colorAttachment, depthAttachment };
}

auto GraphicsPipeline::createRenderPass() -> vk::raii::RenderPass {
    // color attachment reference
    const auto colorAttachmentRef = vk::AttachmentReference {
        .attachment = 0,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    };

    // depth attachment reference
    const auto depthAttachmentRef = vk::AttachmentReference {
        .attachment = 1,
        .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    // subpass
    const auto subpass = vk::SubpassDescription {
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef,
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
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments = attachments.data(),
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &subpassDependency,
    };

    return { device.getLogicalDevice(), renderPassCreateInfo };
}

auto GraphicsPipeline::createPipelineLayout() -> vk::raii::PipelineLayout {
    // setup pipeline layout create info
    // we use the descriptor set for the uniform buffer
    const auto pipelineLayoutCreateInfo =
        vk::PipelineLayoutCreateInfo { .setLayoutCount = 1,
                                       .pSetLayouts = &*descriptorSetLayout.getDescriptorSetLayout() };
    return vk::raii::PipelineLayout { device.getLogicalDevice(), pipelineLayoutCreateInfo };
}

auto GraphicsPipeline::createPipeline() -> vk::raii::Pipeline {
    // setup shader stages create infos
    const auto vertexShaderStageInfo = vk::PipelineShaderStageCreateInfo { .stage = vk::ShaderStageFlagBits::eVertex,
                                                                           .module = *vertexShader.getShaderModule(),
                                                                           .pName = "main" };

    const auto fragmentShaderStageInfo =
        vk::PipelineShaderStageCreateInfo { .stage = vk::ShaderStageFlagBits::eFragment,
                                            .module = *fragmentShader.getShaderModule(),
                                            .pName = "main" };


    const auto pipelineShaderStageCreateInfos = std::array {
        vertexShaderStageInfo,
        fragmentShaderStageInfo,
    };


    // setup dynamic states create infos
    const auto dynamicStates = std::array { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
    const auto pipelineDynamicStateCreateInfo =
        vk::PipelineDynamicStateCreateInfo { .dynamicStateCount = dynamicStates.size(),
                                             .pDynamicStates =
                                                 reinterpret_cast<const vk::DynamicState*>(&dynamicStates) };

    const auto vertexInputBindingDescription = VertexObject::getBindingDescription();
    const auto vertexAttributeDescription = VertexObject::getAttributeDescriptions();

    // setup vertex input state create info
    const auto pipelineVertexInputStateCreateInfo = vk::PipelineVertexInputStateCreateInfo {
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &vertexInputBindingDescription,
        .vertexAttributeDescriptionCount = vertexAttributeDescription.size(),
        .pVertexAttributeDescriptions = vertexAttributeDescription.data(),
    };

    // we are just drawing triangles for now,
    // so we just use a trinagle list for the input assembly
    const auto pipelineInputAssemblyStateCreateInfo = vk::PipelineInputAssemblyStateCreateInfo {
        .topology = vk::PrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = VK_FALSE,
    };

    // setup viewport state create info
    const auto pipelineViewportStateCreateInfo =
        vk::PipelineViewportStateCreateInfo { .viewportCount = 1, .scissorCount = 1 };

    // setup rasterization state create info
    const auto pipelineRasterizationStateCreateInfo = vk::PipelineRasterizationStateCreateInfo {
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = vk::PolygonMode::eFill,  // regular fragment filling, as opposed to wireframe
        .cullMode = vk::CullModeFlagBits::eBack,
        .frontFace = vk::FrontFace::eCounterClockwise,
        .depthBiasEnable = VK_FALSE,
        .lineWidth = 1.0F,
    };

    // setup multisample state create info
    const auto pipelineMultisampleStateCreateInfo = vk::PipelineMultisampleStateCreateInfo {
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = VK_FALSE,
    };

    // depth stencil state create info
    const auto pipelineDepthStencilStateCreateInfo = vk::PipelineDepthStencilStateCreateInfo {
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = vk::CompareOp::eLess,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,  // no stencil testing for now
        .minDepthBounds = 0.0F,
        .maxDepthBounds = 1.0F,
    };

    // setup color blending state create info
    const auto pipelineColorBlendAttachmentState = vk::PipelineColorBlendAttachmentState {
        .blendEnable = VK_FALSE,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
                          | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
    };
    const auto pipelineColorBlendStateCreateInfo = vk::PipelineColorBlendStateCreateInfo {
        .logicOpEnable = VK_FALSE,
        .attachmentCount = 1,
        .pAttachments = &pipelineColorBlendAttachmentState,
    };

    // finally, create the pipeline
    // create the pipeline create info
    const auto pipelineCreateInfo = vk::GraphicsPipelineCreateInfo {
        .stageCount = pipelineShaderStageCreateInfos.size(),
        .pStages = reinterpret_cast<const vk::PipelineShaderStageCreateInfo*>(pipelineShaderStageCreateInfos.data()),
        .pVertexInputState = &pipelineVertexInputStateCreateInfo,
        .pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo,
        .pViewportState = &pipelineViewportStateCreateInfo,
        .pRasterizationState = &pipelineRasterizationStateCreateInfo,
        .pMultisampleState = &pipelineMultisampleStateCreateInfo,
        .pDepthStencilState = &pipelineDepthStencilStateCreateInfo,
        .pColorBlendState = &pipelineColorBlendStateCreateInfo,
        .pDynamicState = &pipelineDynamicStateCreateInfo,
        .layout = *pipelineLayout,
        .renderPass = *renderPass,
        .subpass = 0,
    };

    return vk::raii::Pipeline { device.getLogicalDevice(), nullptr, pipelineCreateInfo };
}

}  // namespace global_ao