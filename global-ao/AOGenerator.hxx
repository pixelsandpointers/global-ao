#pragma once
#include "BVH.hxx"

class AOGenerator{
    private:
        BVH bvh;
        inline glm::vec3 spherePoint();
    public:
        bool bake(int numSamples); // tests for numSamples rays if they intersect the model to compute AO
    auto getVertices(){return bvh.verts;};
    auto getIndices(){return bvh.tris;};
    AOGenerator(BVH& bvh);
    AOGenerator(TriangleMesh* mesh);
    ~AOGenerator();

};