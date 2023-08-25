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
    int startTriOffset = 0;
    int numTri = 0;
};

class BVH
{
private:
    unsigned int triLimit = 400;    // Maximum number of triangles allowed for a leaf node. Tweak for performance
    
public:
    std::vector<Node> build_nodes;              // bvh structure only used during build
    std::vector<RenderNode> render_nodes;       // bvh structure with info used for generating ao
    std::vector<Vertex> verts;                  // full copy of mode vertices
    std::vector<glm::vec3> verts_pos;           // copy of only the position attribute for faster Triangle tests
    std::vector<Triangle> tris;                 // copy of triangles
    std::vector<uint32_t> perNodeTriIndices;// flat array that contains the indices of triangles for each node at an offset
    BVH(){};
    BVH(std::vector<Vertex>* vertices, std::vector<Triangle>* triangles);
    ~BVH();

    void build(); // build the bvh structure
    bool rayAABBTest(AABB& aabb, glm::vec3 origin, glm::vec3 dir);
    bool rayTriangleTest(glm::vec3 origin, glm::vec3 direction, glm::uint index, bool backfaceCulling=true);
    bool collissionCheck(glm::vec3 origin, glm::vec3 dir); // test a Ray for collision with the model using the BVH structure 
};