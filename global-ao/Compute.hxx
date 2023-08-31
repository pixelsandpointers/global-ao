#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "AOGeneratorCPU.hxx"
#include "BVH.hxx"

struct RenderNodeCompute{
    float aabb_min_x, aabb_min_y, aabb_min_z;
    float aabb_max_x, aabb_max_y, aabb_max_z;
    int left = -1;
    int right = -1;
    int startTriOffset = 0;
    int numTri = 0;
};

class AOCompute
{
private:
    GLuint ID;
    GLuint ssbo_vertices, ssbo_vertex_normals, ssbo_triangles, ssbo_aoOutput, ssbo_renderNodes, ssbo_perNodeTriIndices;
    bool useBVH;
public:
    AOCompute(bool useBVH, const char* compPath = "../../global-ao/shader/AOComputeFull.comp");
    ~AOCompute();
    void run(BVH &bvh);
};

