#include <catch2/catch_test_macros.hpp>
#include <glm/glm.hpp>

TEST_CASE("GLM is working") {
    glm::vec3 a(1, 2, 3);
    glm::vec3 b(1, 2, 3);
    auto c = a + b;
    REQUIRE(c.x == 2);
    REQUIRE(c.y == 4);
    REQUIRE(c.z == 6);
}
