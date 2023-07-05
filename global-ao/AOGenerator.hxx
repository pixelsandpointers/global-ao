#pragma once
#include "BVH.hxx"

class AOGenerator{
    private:
        BVH* bvh;
        inline glm::vec3 spherePoint();
    public:
        bool bake(int numSamples);
    AOGenerator(BVH* bvh);
    ~AOGenerator();

};