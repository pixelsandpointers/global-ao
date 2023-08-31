#pragma once

#include "ShaderProgram.hxx"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texcoord
    glm::vec2 Texcoord;
    // occlusion
    glm::vec4 Occlusion = glm::vec4(0.0);
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

    std::vector<Vertex>& GetVertices() {
        return m_vertices;
    };

    /// renders the mesh
    void Draw() const;

    /// updates vertex buffer
    void UpdateBuffers() const;

  private:
    /// setup the vertex and element buffer
    void setupBuffers();
};