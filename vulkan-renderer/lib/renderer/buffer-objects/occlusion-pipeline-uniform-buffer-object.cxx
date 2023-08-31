#include <lib/renderer/buffer-objects/occlusion-pipeline-uniform-buffer-object.hxx>

namespace global_ao {

auto OcclusionPipelineUniformBufferFragmentObject::getBindingDescription() -> vk::DescriptorSetLayoutBinding {
    return {
        .binding = 2,
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = 1,
        .stageFlags = vk::ShaderStageFlagBits::eFragment,
    };
}
}  // namespace global_ao
