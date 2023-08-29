#pragma once

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
    friend class Model;
  private:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    unsigned int m_VAO, m_VBO, m_EBO;
    unsigned int m_numIndices;

  public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    ~Mesh() = default;

    std::vector<Vertex>& GetVertices();

    /// renders the mesh
    void Draw() const;

    /// 
    void UpdateBuffers();

  private:
    /// setup the vertex and element buffer
    void setupBuffers();
};