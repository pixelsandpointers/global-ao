#include <lib/renderer/renderer.hxx>
#include <lib/window/window.hxx>


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
    const auto window = Window { 800, 600 };
    auto renderer = VulkanRenderer { window };
    mainLoop(window, renderer);
    return 0;
}
