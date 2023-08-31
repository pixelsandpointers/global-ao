#pragma once
#include "BVH.hxx"

class AOGeneratorCPU{
    private:
        BVH bvh;
        inline glm::vec3 spherePoint();
    public:
        bool bake(int numSamples); // tests for numSamples rays if they intersect the model to compute AO
        bool bake(int numSamples, std::vector<float>& spherePoints, std::vector<float>& hemiDirs);
    auto getVertices(){return bvh.verts;};
    auto getIndices(){return bvh.tris;};
    AOGeneratorCPU(BVH& bvh);
    AOGeneratorCPU(TriangleMesh* mesh);
    ~AOGeneratorCPU();

};