#pragma once
#include <lib/renderer/shader-module.hxx>
#include <lib/renderer/swap-chain-provider.hxx>

namespace global_ao {

class GraphicsPipeline {
  public:
    GraphicsPipeline(const Device& device, const SwapChainHandler& imageViewProvider);

    auto getRenderPass() const -> const vk::raii::RenderPass& {
        return renderPass;
    }

    auto getPipeline() const -> const vk::raii::Pipeline& {
        return pipeline;
    }

  private:
    auto createRenderPass() -> vk::raii::RenderPass;
    auto createPipeline() -> vk::raii::Pipeline;

    const Device& device;
    const SwapChainHandler& imageViewProvider;
    const ShaderModule vertexShader;
    const ShaderModule fragmentShader;
    vk::raii::RenderPass renderPass;
    vk::raii::Pipeline pipeline;
};

}  // namespace global_ao
