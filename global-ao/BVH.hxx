#pragma once
#include "TriangleMesh.hxx"

struct AABB
{
    glm::vec3 min, max;
};

struct Node
{
    AABB aabb;
    unsigned int parent;
    int left = -1;
    int right = -1;
    std::vector<unsigned int> triangles;
};

class BVH
{
private:
    unsigned int triLimit = 400;
    
public:
    std::vector<Node> nodes;
    std::vector<Vertex> verts;
    std::vector<Triangle> tris;
    BVH(std::vector<Vertex>* vertices, std::vector<Triangle>* triangles);
    ~BVH();

    void build();
};