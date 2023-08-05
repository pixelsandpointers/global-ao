#include <catch2/catch_test_macros.hpp>
#include <lib/renderer/renderer.hxx>
#include <lib/window/window.hxx>

using namespace global_ao;

TEST_CASE("Renderer is created", "[renderer]") {
    const auto window = Window { 800, 600 };
    auto renderer = VulkanRenderer(window);
    REQUIRE(true);
}