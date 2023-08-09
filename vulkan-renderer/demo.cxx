#include <lib/renderer/renderer.hxx>


using global_ao::Vertex;
using global_ao::VulkanRenderer;
using global_ao::Window;

void mainLoop(const Window& window, VulkanRenderer& renderer) {
    while (!glfwWindowShouldClose(window.getWindow())) {
        glfwPollEvents();
        renderer.drawFrame();
    }
    renderer.waitIdle();
}

int main() {
    const std::vector<Vertex> vertices = {
        {.pos = { 0.0F, -0.5F }, .color = { 1.0F, 1.0F, 1.0F }},
        { .pos = { 0.5F, 0.5F }, .color = { 0.0F, 1.0F, 0.0F }},
        {.pos = { -0.5F, 0.5F }, .color = { 0.0F, 0.0F, 1.0F }},
    };

    const auto window = Window { 800, 600 };

    auto renderer = VulkanRenderer { window };
    renderer.loadVertices(vertices);
    mainLoop(window, renderer);
    return 0;
}
