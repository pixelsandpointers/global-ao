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


glm::vec3 spherePoint(){
    for (int i = 0; i < 10; ++i){
        float x = 2.0*float(rand())/float(RAND_MAX)-1.0;
        float y = 2.0*float(rand())/float(RAND_MAX)-1.0;
        float z = 2.0*float(rand())/float(RAND_MAX)-1.0;
        glm::vec3 dir = glm::vec3(x, y, z);
        if (glm::length(dir) < 1.0)
        {
            glm::normalize(dir);
            return dir;
        }
    }
    return glm::vec3(0, 0, 0);
}


bool layerOutput(std::vector<Vertex>* vertices, std::vector<Triangle>* triangles) {
    for(int i = 0; i < vertices->size(); ++i){
        auto& vtx = (*vertices)[i];
        int samples = 2;
        int sum = 0;
        for (int d = 0; d < samples; ++d){
            bool hitFound = false;
            auto dir = spherePoint();
            dir = glm::dot(dir, vtx.normal)<0?-dir:dir;
            for (int t = 0; t < triangles->size(); ++t){
                auto hit = rayTriangleTest(vtx.position, dir, t, vertices, triangles);
                hitFound |= hit;
            }
            if (hitFound) ++sum;
        }
        float value = float(sum)/float(samples);
        vtx.color = glm::vec4(1.0f-value, 1.0f-value, 1.0f-value, 1.0);
    }
    return true;
}