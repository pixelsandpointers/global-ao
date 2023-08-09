#include <lib/renderer/graphics-pipeline.hxx>
#include <lib/renderer/vertex.hxx>

namespace global_ao {
GraphicsPipeline::GraphicsPipeline(const Device& device, const SwapChainHandler& imageViewProvider)
  : device { device },
    imageViewProvider { imageViewProvider },
    vertexShader { shader_vert_PATH, device },
    fragmentShader { shader_frag_PATH, device },
    renderPass { createRenderPass() },
    pipeline { createPipeline() } {
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

    const auto vertexInputBindingDescription = Vertex::getBindingDescription();
    const auto vertexAttributeDescription = Vertex::getAttributeDescriptions();

    // setup vertex input state create info
    const auto pipelineVertexInputStateCreateInfo = vk::PipelineVertexInputStateCreateInfo {
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &vertexInputBindingDescription,
        .vertexAttributeDescriptionCount = 2,
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
        .frontFace = vk::FrontFace::eClockwise,
        .depthBiasEnable = VK_FALSE,
        .lineWidth = 1.0F,
    };

    // setup multisample state create info
    const auto pipelineMultisampleStateCreateInfo = vk::PipelineMultisampleStateCreateInfo {
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = VK_FALSE,
    };

    // no depth and stencil testing for now

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

    // setup pipeline layout create info
    // which is just empty for now
    const auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo {};
    const auto pipelineLayout = vk::raii::PipelineLayout { device.getLogicalDevice(), pipelineLayoutCreateInfo };

    // setup render pass create info

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
        .pColorBlendState = &pipelineColorBlendStateCreateInfo,
        .pDynamicState = &pipelineDynamicStateCreateInfo,
        .layout = *pipelineLayout,
        .renderPass = *renderPass,
        .subpass = 0,
    };

    return vk::raii::Pipeline { device.getLogicalDevice(), nullptr, pipelineCreateInfo };
}

auto GraphicsPipeline::createRenderPass() -> vk::raii::RenderPass {
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
    };

    // subpass dependency
    const auto subpassDependency = vk::SubpassDependency {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .srcAccessMask = vk::AccessFlagBits::eNoneKHR,
        .dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
    };

    // actual render pass
    const auto renderPassCreateInfo = vk::RenderPassCreateInfo {
        .attachmentCount = 1,
        .pAttachments = &colorAttachment,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &subpassDependency,
    };

    return { device.getLogicalDevice(), renderPassCreateInfo };
}
}  // namespace global_ao