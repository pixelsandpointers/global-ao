#include <lib/renderer/buffers/uniform-buffer-object.hxx>

namespace global_ao {
auto UniformBufferObject::getBindingDescription() -> vk::DescriptorSetLayoutBinding {
    return { .binding = 0,
             .descriptorType = vk::DescriptorType::eUniformBuffer,
             .descriptorCount = 1,
             .stageFlags = vk::ShaderStageFlagBits::eVertex };
}


}  // namespace global_ao
