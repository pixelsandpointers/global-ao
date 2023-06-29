#pragma once
#include "TriangleMesh.hxx"
#include "BVH.hxx"

bool rayAABBTest(AABB& aabb, glm::vec3 origin, glm::vec3 dir);

bool rayTriangleTest(glm::vec3 origin, glm::vec3 direction, glm::uint index, std::vector<glm::vec3>* vertices, std::vector<Triangle>* triangles);

float rayTracing(std::vector<Vertex>* vertices, std::vector<Triangle>* triangles, Vertex& vtx, int samples);

glm::vec3 spherePoint();

bool layerOutput(std::vector<Vertex>* vertices, std::vector<Triangle>* triangles);

bool bvhAO(BVH& bvh, int samples);