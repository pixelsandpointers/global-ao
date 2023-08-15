#pragma once
#include <lib/renderer/descriptor-set-layout.hxx>
#include <lib/renderer/shader-module.hxx>
#include <lib/renderer/swap-chain-provider.hxx>

namespace global_ao {

class GraphicsPipeline {
  public:
    GraphicsPipeline(
        const Device& device,
        const SwapChainHandler& imageViewProvider,
        const DescriptorSetLayout& descriptorSetLayout);

    auto getRenderPass() const -> const vk::raii::RenderPass&;

    auto getPipeline() const -> const vk::raii::Pipeline&;

    auto getPipelineLayout() const -> const vk::raii::PipelineLayout&;

  private:
    auto createRenderPass() -> vk::raii::RenderPass;
    auto createPipelineLayout() -> vk::raii::PipelineLayout;
    auto createPipeline() -> vk::raii::Pipeline;

    const Device& device;
    const SwapChainHandler& imageViewProvider;
    const DescriptorSetLayout& descriptorSetLayout;
    const ShaderModule vertexShader;
    const ShaderModule fragmentShader;
    vk::raii::RenderPass renderPass;
    vk::raii::PipelineLayout pipelineLayout;
    vk::raii::Pipeline pipeline;
};

}  // namespace global_ao
