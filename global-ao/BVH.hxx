#pragma once
#include "TriangleMesh.hxx"
#include <numeric>

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

struct RenderNode{
    AABB aabb;
    int left = -1;
    int right = -1;
    int startTri = 0;
    int numTri = 0;
};


class BVH
{
private:
    unsigned int triLimit = 400;
    
public:
    std::vector<Node> nodes;
    std::vector<RenderNode> render_nodes;
    std::vector<Vertex> verts;
    std::vector<glm::vec3> verts_pos;
    std::vector<Triangle> tris;
    std::vector<unsigned int> perNodeTris;
    BVH(std::vector<Vertex>* vertices, std::vector<Triangle>* triangles);
    ~BVH();

    void buildManager(bool withRender);
    void build(){buildManager(false);};
    void buildWithRender(){buildManager(true);};
};