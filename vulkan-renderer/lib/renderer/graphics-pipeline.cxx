#include <lib/renderer/buffer-objects/vertex-object.hxx>

#include <lib/renderer/graphics-pipeline.hxx>
#include <utility>

namespace global_ao {
GraphicsPipeline::GraphicsPipeline(
    const Device& device,
    const vk::Format& format,
    const DescriptorSetLayouts& descriptorSetLayouts,
    const std::filesystem::path& vertexShaderPath,
    const std::filesystem::path& fragmentShaderPath,
    vk::raii::RenderPass renderPass)
  : device { device },
    descriptorSetLayouts { descriptorSetLayouts },
    vertexShader { device, vertexShaderPath },
    fragmentShader { device, fragmentShaderPath },
    renderPass { std::move(renderPass) },
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

auto GraphicsPipeline::createPipelineLayout() -> vk::raii::PipelineLayout {
    // setup pipeline layout create info
    // we use the descriptor set for the uniform buffer
    const auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo {
        .setLayoutCount = 1,
        .pSetLayouts = &*descriptorSetLayouts.getDescriptorSetLayouts()[0]
    };
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

    const auto vertexInputBindingDescription = VertexObject::getBindingDescription();  // TODO: take this as a parameter
    const auto vertexAttributeDescription = VertexObject::getAttributeDescriptions();  // TODO: take this as a parameter
    // regarding the TODO: I believe that ideally there should be some datastructure to define all sorts of pipeline input
    // like vertex input, texture coordinates, uniform buffers, and texture samplers, etc...
    // this would then hold all the needed information to create the pipeline and the descriptor sets

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