#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

struct VertexObject {
    glm::vec2 pos;
    glm::vec3 color;

    static auto getBindingDescription() -> vk::VertexInputBindingDescription;

    static auto getAttributeDescriptions() -> std::array<vk::VertexInputAttributeDescription, 2>;
};

}  // namespace global_ao
