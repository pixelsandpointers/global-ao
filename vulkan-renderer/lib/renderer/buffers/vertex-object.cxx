#include <lib/renderer/buffers/vertex-object.hxx>

namespace global_ao {
auto VertexObject::getBindingDescription() -> vk::VertexInputBindingDescription {
    return {
        .binding = 0,
        .stride = sizeof(VertexObject),
        .inputRate = vk::VertexInputRate::eVertex,
    };
}

auto VertexObject::getAttributeDescriptions() -> std::array<vk::VertexInputAttributeDescription, 3> {
    return {
        vk::VertexInputAttributeDescription {
                                             .location = 0,
                                             .binding = 0,
                                             .format = vk::Format::eR32G32B32Sfloat,
                                             .offset = offsetof(VertexObject,               pos),
                                             },
        vk::VertexInputAttributeDescription {
                                             .location = 1,
                                             .binding = 0,
                                             .format = vk::Format::eR32G32B32Sfloat,
                                             .offset = offsetof(VertexObject,             color),
                                             },
        vk::VertexInputAttributeDescription {
                                             .location = 2,
                                             .binding = 0,
                                             .format = vk::Format::eR32G32Sfloat,
                                             .offset = offsetof(VertexObject, textureCoordinate),
                                             },
    };
}
}  // namespace global_ao