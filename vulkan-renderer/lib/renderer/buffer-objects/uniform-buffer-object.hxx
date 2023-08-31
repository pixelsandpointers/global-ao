#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {
struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;

    static auto getBindingDescription() -> vk::DescriptorSetLayoutBinding;
};

}  // namespace global_ao