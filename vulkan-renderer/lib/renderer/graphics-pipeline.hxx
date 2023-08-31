#pragma once
#include <lib/renderer/depth-resources.hxx>
#include <lib/renderer/descriptor-set-layouts.hxx>
#include <lib/renderer/shader-module.hxx>
#include <lib/renderer/swap-chain-provider.hxx>

namespace global_ao {

class GraphicsPipeline {
  public:
    GraphicsPipeline(
        const Device& device,
        const vk::Format& format,
        const DescriptorSetLayouts& descriptorSetLayouts,
        const std::filesystem::path& vertexShaderPath,
        const std::filesystem::path& fragmentShaderPath,
        vk::raii::RenderPass renderPass);

    auto getRenderPass() const -> const vk::raii::RenderPass&;
    auto getPipeline() const -> const vk::raii::Pipeline&;
    auto getPipelineLayout() const -> const vk::raii::PipelineLayout&;

  private:
    auto createPipelineLayout() -> vk::raii::PipelineLayout;
    auto createPipeline() -> vk::raii::Pipeline;

    const Device& device;
    const DescriptorSetLayouts& descriptorSetLayouts;
    const ShaderModule vertexShader;
    const ShaderModule fragmentShader;
    const std::vector<vk::AttachmentDescription> attachmentDescriptions;
    const std::vector<vk::AttachmentReference> attachmentReferences;
    const vk::raii::RenderPass renderPass;
    const vk::raii::PipelineLayout pipelineLayout;
    const vk::raii::Pipeline pipeline;
};

}  // namespace global_ao
