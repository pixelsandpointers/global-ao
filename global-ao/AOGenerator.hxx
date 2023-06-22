#pragma once
#include "TriangleMesh.hxx"

bool rayTriangleTest(glm::vec3 origin, glm::vec3 direction, glm::uint index, std::vector<Vertex>* vertices, std::vector<Triangle>* triangles);

glm::vec3 spherePoint();

bool layerOutput(std::vector<Vertex>* vertices, std::vector<Triangle>* triangles);