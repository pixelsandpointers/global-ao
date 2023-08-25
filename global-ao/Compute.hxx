#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "BVH.hxx"

struct RendeNodeCompute{
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
    GLuint ssbo_vertices, ssbo_vertex_normals, ssbo_triangles, ssbo_aoOutput, ssbo_renderNodes, ssbo_perNodeTriIndices, ssbo_debug_random;
public:
    AOCompute(const char* compPath);
    ~AOCompute();
    void run(BVH &bvh);
};

AOCompute::AOCompute(const char* compPath = "../../global-ao/shader/test.comp")
{
    std::string shaderString;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    int success;
    char infoLog[1024];
    try
    {
        file.open(compPath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        shaderString = buffer.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER - failed to read compute shader:\n" << e.what() << std::endl;
    }

    const char* cShaderCode = shaderString.c_str();
    GLuint compute;
    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &cShaderCode, NULL);
    glCompileShader(compute);

    glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(compute, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER - failed to compile compute shader:\n" << infoLog << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);


    // create buffer
    glGenBuffers(1, &ssbo_vertices);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_vertices);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_vertices);

    glGenBuffers(1, &ssbo_vertex_normals);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_vertex_normals);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo_vertex_normals);

    glGenBuffers(1, &ssbo_triangles);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_triangles);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo_triangles);

    glGenBuffers(1, &ssbo_aoOutput);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_aoOutput);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo_aoOutput);

    glGenBuffers(1, &ssbo_renderNodes);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_renderNodes);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, ssbo_renderNodes);

    glGenBuffers(1, &ssbo_perNodeTriIndices);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_perNodeTriIndices);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, ssbo_perNodeTriIndices);

    glGenBuffers(1, &ssbo_debug_random);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_debug_random);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, ssbo_debug_random);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

AOCompute::~AOCompute()
{
}


void AOCompute::run(BVH &bvh)
{
    std::vector<uint32_t> aoOutput(bvh.verts_pos.size(), 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_aoOutput);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t)*aoOutput.size(),aoOutput.data(),  GL_DYNAMIC_COPY);
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_vertices);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec3)*bvh.verts_pos.size(), bvh.verts_pos.data(),  GL_DYNAMIC_COPY);

    std::vector<glm::vec3> normals(bvh.verts_pos.size(), glm::vec3(0));
    for (size_t i = 0; i < bvh.verts_pos.size(); ++i) normals[i] = bvh.verts[i].normal;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_vertex_normals);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 3*sizeof(float)*bvh.verts_pos.size(), bvh.verts_pos.data(),  GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_triangles);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 3*sizeof(float)*bvh.tris.size(), bvh.tris.data(),  GL_DYNAMIC_COPY);

    // flatten Render Nodes
    std::vector<RendeNodeCompute> renderNodesCompute(bvh.render_nodes.size());
    for (size_t i = 0; i < bvh.render_nodes.size(); ++i){
        renderNodesCompute[i].aabb_min_x = bvh.render_nodes[i].aabb.min.x;
        renderNodesCompute[i].aabb_min_y = bvh.render_nodes[i].aabb.min.y;
        renderNodesCompute[i].aabb_min_z = bvh.render_nodes[i].aabb.min.z;
        renderNodesCompute[i].aabb_max_x = bvh.render_nodes[i].aabb.max.x;
        renderNodesCompute[i].aabb_max_y = bvh.render_nodes[i].aabb.max.y;
        renderNodesCompute[i].aabb_max_z = bvh.render_nodes[i].aabb.max.z;
        renderNodesCompute[i].left = bvh.render_nodes[i].left;
        renderNodesCompute[i].right = bvh.render_nodes[i].right;
        renderNodesCompute[i].startTriOffset = bvh.render_nodes[i].startTriOffset;
        renderNodesCompute[i].numTri = bvh.render_nodes[i].numTri;
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_renderNodes);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(RendeNodeCompute)*bvh.render_nodes.size(), bvh.render_nodes.data(),  GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_perNodeTriIndices);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t)*bvh.perNodeTriIndices.size(), bvh.perNodeTriIndices.data(),  GL_DYNAMIC_COPY);

    std::vector<float> debug_random(3*bvh.verts_pos.size());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_debug_random);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float)*debug_random.size(), debug_random.data(),  GL_DYNAMIC_COPY);
    
    size_t numIters = 1;
    GLint numSamples[3];
    
    { // launch compute shaders!
    glUseProgram(ID);

    auto a = glGetUniformLocation(ID, "num_verts");
    glUniform1ui(glGetUniformLocation(ID, "num_verts"), bvh.verts_pos.size());

    auto b = glGetUniformLocation(ID, "num_tris");
    glUniform1ui(glGetUniformLocation(ID, "num_tris"), bvh.tris.size());

    glGetProgramiv(ID, GL_COMPUTE_WORK_GROUP_SIZE, numSamples);

    for (size_t i = 0; i < numIters; ++i) glDispatchCompute(bvh.verts_pos.size(), 1, 1);
    }

   
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_aoOutput);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float)*aoOutput.size(), aoOutput.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_debug_random);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float)*debug_random.size(), debug_random.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    for (size_t i = 0; i < bvh.verts_pos.size(); ++i){
        float val = 1.0f-float(aoOutput[i])/float(numIters*numSamples[1]);
        bvh.verts[i].color = glm::vec4(val, val, val, 1.0f);
    } 
}