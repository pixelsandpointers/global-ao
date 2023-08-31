#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <lib/renderer/ambient-occlusion/ambient-occlusion.hxx>

namespace global_ao {
AmbientOcclusion::AmbientOcclusion(const Device& device, vk::Extent2D extent, uint32_t sampleSize, const std::unique_ptr<VertexBuffer>& vertexBuffer, const std::unique_ptr<IndexBuffer>& indexBuffer)
  : generator { rd() },
    device { device },
    extent { extent },
    sampleSize { sampleSize },
    vertexBuffer { vertexBuffer },
    indexBuffer { indexBuffer },
    ubo {},
    uniformBuffers { createUniformBuffers() },
    depthPipeline {device,vk::Format::eB8G8R8A8Srgb,vk::Extent2D { extent.width, extent.height },ubo, uniformBuffers,indexBuffer,vertexBuffer},
    occlusionPipelineUbo { .nSamples=static_cast<float>(sampleSize) },
    occlusionUniformBuffers { createOcclusionUniformBuffers()   },
    occlusionPipeline {device,ubo ,uniformBuffers, occlusionPipelineUbo, occlusionUniformBuffers,depthPipeline.getDepthResources(),vertexBuffer,indexBuffer},
    accumulationPipeline {device, vertexBuffer, indexBuffer, occlusionPipeline.getOcclusionImage()} {
}

auto AmbientOcclusion::computeOcclusion(const UniformBufferObject& newUbo) -> void {
    ubo = newUbo;
    ubo.view = glm::lookAt(glm::vec3(2.0F, 2.0F, 2.0F), glm::vec3(0.0F, 0.0F, 0.0F), glm::vec3(0.0F, 0.0F, 1.0F));
    ubo.model = glm::rotate(glm::mat4(1.0F), glm::radians(90.0F), glm::vec3(0.0F, 0.0F, 1.0F));
    ubo.proj = glm::perspective(
        glm::radians(45.0F),
        static_cast<float>(extent.width) / static_cast<float>(extent.height),
        0.1F,
        10.0F);
//
    ubo.proj[1][1] *= -1;  // invert y coordinate
    auto lightViewMatrix = glm::mat4(1.0f);

    for (auto i = 0; i < sampleSize; i++) {
        auto x_rotation = distribution(generator) * 360.0F;
        auto y_rotation = distribution(generator) * 360.0F;
        auto z_rotation = distribution(generator) * 360.0F;

        lightViewMatrix = glm::rotate(lightViewMatrix, x_rotation, glm::vec3 { 1.0F, 0.0F, 0.0F });
        lightViewMatrix = glm::rotate(lightViewMatrix, y_rotation, glm::vec3 { 0.0F, 1.0F, 0.0F });
        lightViewMatrix = glm::rotate(lightViewMatrix, z_rotation, glm::vec3 { 0.0F, 0.0F, 1.0F });
        ubo.view = lightViewMatrix;

        // TODO: pass ubo to render call at the moment it is implicitly passed through a reference in the constructor
        depthPipeline.render();
        device.waitIdle();
        occlusionPipeline.render();
        device.waitIdle();
        accumulationPipeline.render(i % 2);
        device.waitIdle();
    }
}

auto AmbientOcclusion::createUniformBuffers() -> std::vector<UniformBuffer<UniformBufferObject>> {
    auto _uniformBuffers = std::vector<UniformBuffer<UniformBufferObject>> {};
    _uniformBuffers.reserve(1);
    _uniformBuffers.emplace_back(device);
    return _uniformBuffers;
}

auto AmbientOcclusion::createOcclusionUniformBuffers()
    -> std::vector<UniformBuffer<OcclusionPipelineUniformBufferFragmentObject>> {
    auto _uniformBuffers = std::vector<UniformBuffer<OcclusionPipelineUniformBufferFragmentObject>> {};
    _uniformBuffers.reserve(1);
    _uniformBuffers.emplace_back(device);
    return _uniformBuffers;
}


}  // namespace global_ao