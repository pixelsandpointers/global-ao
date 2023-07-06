#include "AOGenerator.hxx"
#include <queue>

AOGenerator::AOGenerator(BVH& bvh){
    this->bvh = bvh;
}

AOGenerator::AOGenerator(TriangleMesh* mesh) {
    bvh = BVH(mesh->getVertices(), mesh->getIndices());
    bvh.build();
}

AOGenerator::~AOGenerator() {}

inline glm::vec3 AOGenerator::spherePoint() {
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

bool AOGenerator::bake(int numSamples) {
    for (auto& vtx : bvh.verts){
        unsigned int sum = 0;
        for (int d = 0; d < numSamples; ++d){
            auto hemidir = spherePoint();
            hemidir = glm::dot(hemidir, vtx.normal)<0?-hemidir:hemidir;
            if (bvh.collissionCheck(vtx.position, hemidir)) ++sum;
        }
        auto val = 1.0 - float(sum)/float(numSamples);
        vtx.color = glm::vec4(val, val, val, 1);
    }
    return true;
}


