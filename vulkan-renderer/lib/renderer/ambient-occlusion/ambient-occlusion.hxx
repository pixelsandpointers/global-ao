#pragma once

#include <cstdint>
#include <lib/renderer/ambient-occlusion/pipelines/accumulation-pipeline.hxx>
#include <lib/renderer/ambient-occlusion/pipelines/depth-pipeline.hxx>
#include <lib/renderer/ambient-occlusion/pipelines/occlusion-pipeline.hxx>
#include <lib/renderer/buffer-objects/depth-pipeline-uniform-buffer-object.hxx>
#include <lib/renderer/buffers/image.hxx>
#include <lib/renderer/descriptor-sets.txx>
#include <lib/renderer/device.hxx>
#include <random>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class AmbientOcclusion {
  public:
    AmbientOcclusion(
        const Device& device,
        vk::Extent2D extent,
        uint32_t sampleSize,
        const std::unique_ptr<VertexBuffer>& vertexBuffer,
        const std::unique_ptr<IndexBuffer>& indexBuffer);

    auto computeOcclusion(const UniformBufferObject& newUbo) -> void;
    auto getOcclusionImage() const -> const Image& {
        return accumulationPipeline.getAccumulationImage();
    }

  private:
    auto createUniformBuffers() -> std::vector<UniformBuffer<UniformBufferObject>>;
    auto createOcclusionUniformBuffers() -> std::vector<UniformBuffer<OcclusionPipelineUniformBufferFragmentObject>>;

    // random number generator
    inline static std::random_device rd {};
    std::default_random_engine generator;
    inline static std::uniform_real_distribution<float> distribution { 0.0, 1.0 };

    const Device& device;
    vk::Extent2D extent;
    uint32_t sampleSize;
    const std::unique_ptr<VertexBuffer>& vertexBuffer;
    const std::unique_ptr<IndexBuffer>& indexBuffer;
    UniformBufferObject ubo;
    const std::vector<UniformBuffer<UniformBufferObject>> uniformBuffers;
    DepthPipeline depthPipeline;
    OcclusionPipelineUniformBufferFragmentObject occlusionPipelineUbo;
    const std::vector<UniformBuffer<OcclusionPipelineUniformBufferFragmentObject>> occlusionUniformBuffers;
    OcclusionPipeline occlusionPipeline;
    AccumulationPipeline accumulationPipeline;
};

}  // namespace global_ao
