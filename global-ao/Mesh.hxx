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
    // constructor
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
        // set data
        m_vertices = vertices;
        m_indices = indices;

        // generate and fill buffers
        glGenVertexArrays(1, &m_VAO);
        updateBuffers();
    }

    ~Mesh() {
        //glDeleteVertexArrays(1, &VAO);  // TODO: Meshes get destroyed somewhere. Why?
    }

    // render the mesh
    void Draw() const {
        // draw mesh
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

  private:
    void updateBuffers() {
        glBindVertexArray(m_VAO);

        m_VBO.Setup(m_vertices.size() * sizeof(Vertex), m_vertices.data());

        m_EBO.Setup(m_indices.size() * sizeof(unsigned int), m_indices.data());
        m_numIndices = static_cast<unsigned int>(m_indices.size());

        // set the vertex attribute pointers
        // vertex positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);
        glEnableVertexAttribArray(0);
        // vertex normals
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);
        // vertex texture coords
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Texcoord));
        glEnableVertexAttribArray(2);
        // vertex colors
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Color));
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);
    }
};