#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static auto getBindingDescription() -> vk::VertexInputBindingDescription {
        return {
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = vk::VertexInputRate::eVertex,
        };
    }

    static auto getAttributeDescriptions() -> std::array<vk::VertexInputAttributeDescription, 2> {
        return {
            vk::VertexInputAttributeDescription {
                                                 .location = 0,
                                                 .binding = 0,
                                                 .format = vk::Format::eR32G32Sfloat,
                                                 .offset = offsetof(Vertex,   pos),
                                                 },
            vk::VertexInputAttributeDescription {
                                                 .location = 1,
                                                 .binding = 0,
                                                 .format = vk::Format::eR32G32B32Sfloat,
                                                 .offset = offsetof(Vertex, color),
                                                 },
        };
    }
};

}  // namespace global_ao
