#include <lib/renderer/renderer.hxx>


using global_ao::VertexObject;
using global_ao::VulkanRenderer;
using global_ao::Window;

void mainLoop(const Window& window, VulkanRenderer& renderer) {
    while (!glfwWindowShouldClose(window.getWindow())) {
        glfwPollEvents();
        renderer.updateUniformBuffer();
        renderer.drawFrame();
    }
    renderer.waitIdle();
}

int main() {
    const auto vertices = std::vector<VertexObject> {
        {.pos = { -0.5F, -0.5F }, .color = { 1.0F, 0.0F, 0.0F }, .textureCoordinate = { 1.0F, 0.0F }},
        { .pos = { 0.5F, -0.5F }, .color = { 0.0F, 1.0F, 0.0F }, .textureCoordinate = { 0.0F, 0.0F }},
        {  .pos = { 0.5F, 0.5F }, .color = { 0.0F, 0.0F, 1.0F }, .textureCoordinate = { 0.0F, 1.0F }},
        { .pos = { -0.5F, 0.5F }, .color = { 1.0F, 1.0F, 1.0F }, .textureCoordinate = { 1.0F, 1.0F }},
    };
    const auto indices = std::vector<uint32_t> { 0, 1, 2, 2, 3, 0 };
    const auto texturePath = std::filesystem::path { TEXTURE_PATH "/texture.jpg" };

    auto window = Window { 800, 600 };

    auto renderer = VulkanRenderer { window };
    renderer.loadVerticesWithIndex(vertices, indices);
    renderer.loadTexture(texturePath);
    renderer.updateDescriptorSets();
    mainLoop(window, renderer);
    return 0;
}
