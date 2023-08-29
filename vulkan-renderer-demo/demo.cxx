#define TINYOBJLOADER_IMPLEMENTATION
#include <lib/renderer/renderer.hxx>
#include <tiny_obj_loader.h>


using global_ao::VertexObject;
using global_ao::VulkanRenderer;
using global_ao::Window;

auto loadModel(const std::filesystem::path modelPath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    auto ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str());

    if (!ret) {
        throw std::runtime_error(warn + err);
    }


    std::vector<VertexObject> vertices;
    std::vector<uint32_t> indices;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            auto vertex = VertexObject {
                .pos = { attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2] },
                .color = { 1.0f, 1.0f, 1.0f },

                .textureCoordinate = { attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1] },
            };
            vertices.push_back(vertex);
            indices.push_back(indices.size());
        }
    }
    return std::make_tuple(vertices, indices);
}

void mainLoop(const Window& window, VulkanRenderer& renderer) {
    while (!glfwWindowShouldClose(window.getWindow())) {
        glfwPollEvents();
        renderer.updateUniformBuffer();
        renderer.drawFrame();
    }
    renderer.waitIdle();
}

int main() {
    const auto [vertices, indices] = loadModel(MODEL_PATH "/viking_room.obj");
    const auto texturePath = std::filesystem::path { TEXTURE_PATH "/viking_room.png" };

    auto window = Window { 800, 600 };

    auto renderer = VulkanRenderer { window };
    renderer.loadVerticesWithIndex(vertices, indices);
    renderer.loadTexture(texturePath);
    renderer.updateDescriptorSets();
    mainLoop(window, renderer);
    return 0;
}
