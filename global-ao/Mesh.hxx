#pragma once

#include "BufferObject.hxx"
#include "ShaderProgram.hxx"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

using Triangle = glm::uvec3;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 Texcoord;
    // color
    glm::vec4 Color = glm::vec4(1.0);
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
  private:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    unsigned int m_VAO;
    VertexBuffer m_VBO;
    ElementBuffer m_EBO;
    unsigned int m_numIndices;

  public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    ~Mesh() {
        //glDeleteVertexArrays(1, &VAO);  // TODO: Meshes get destroyed somewhere. Why?
    }

    /// renders the Mesh
    void Draw() const;

  private:
    /// Updates the vertex and element buffer
    void UpdateBuffers();
};