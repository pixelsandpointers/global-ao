#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

struct VertexObject {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 textureCoordinate;

    static auto getBindingDescription() -> vk::VertexInputBindingDescription;

    static auto getAttributeDescriptions() -> std::array<vk::VertexInputAttributeDescription, 4>;
};

}  // namespace global_ao
