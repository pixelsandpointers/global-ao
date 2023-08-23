#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "BVH.hxx"

class AOCompute
{
private:
    GLuint ID;
    GLuint ssbo_vertices, ssbo_triangles, ssbo_aoOutput;
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

    glGenBuffers(1, &ssbo_triangles);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_triangles);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo_triangles);

    glGenBuffers(1, &ssbo_aoOutput);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_aoOutput);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo_aoOutput);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

AOCompute::~AOCompute()
{
}


void AOCompute::run(BVH &bvh)
{
    std::vector<float> aoOutput(bvh.verts_pos.size(), 1.0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_aoOutput);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float)*aoOutput.size(),aoOutput.data(),  GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_vertices);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 3*sizeof(float)*bvh.verts_pos.size(), bvh.verts_pos.data(),  GL_DYNAMIC_COPY);

    
    { // launch compute shaders!
    glUseProgram(ID);
    glDispatchCompute(1, 1, 1);
    }

   
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_aoOutput);
    // make sure writing to image has finished before read
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float)*aoOutput.size(), aoOutput.data());
    std::cout << aoOutput[0] << "\n";
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}