#include "Mesh.hxx"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
    // set data
    m_vertices = vertices;
    m_indices = indices;

    // generate and fill buffers
    glGenVertexArrays(1, &m_VAO);
    updateBuffers();
}

void Mesh::Draw() const {
    // draw mesh
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::updateBuffers() {
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
