#pragma once
#include "TriangleMesh.hxx"

bool rayTriangleTest(glm::vec3 origin, glm::vec3 direction, glm::uint index, std::vector<Vertex>* vertices, std::vector<Triangle>* triangles){
    // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    const float EPSILON = 0.0000001;
    auto tri = (*triangles)[index];
    glm::vec3 v0 = (*vertices)[tri.x].position;
    glm::vec3 v1 = (*vertices)[tri.y].position;
    glm::vec3 v2 = (*vertices)[tri.z].position;
    glm::vec3 edge1, edge2, h, s, q;
    float a, f, u, v;
    edge1 = v1-v0;
    edge2 = v2-v0;
    h = glm::cross(direction, edge2);
    a = glm::dot(edge1, h);

    if (a > -EPSILON && a < EPSILON)
        return false;    // This ray is parallel to this triangle.

    f = 1.0 / a;
    s = origin - v0;
    u = f * glm::dot(s, h);

    if (u < 0.0 || u > 1.0)
        return false;

    q = glm::cross(s, edge1);
    v = f * dot(direction, q);

    if (v < 0.0 || u + v > 1.0)
        return false;

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * glm::dot(edge2, q);

    if (t > EPSILON) // ray intersection
    {
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}


bool layerOutput(std::vector<Vertex>* vertices, std::vector<Triangle>* triangles) {
    for(int i = 0; i < vertices->size(); ++i){
        auto& vtx = (*vertices)[i];
        bool hitFound = false;
        for (int t = 0; t < triangles->size(); ++t){
            auto hit = rayTriangleTest(vtx.position, vtx.normal, t, vertices, triangles);
            hitFound |= hit;
        }
        
        vtx.color = glm::vec4(1.0f, hitFound?1.0:0.0, 1.0, 1.0);
    }
    return true;
}