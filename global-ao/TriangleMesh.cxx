#include "TriangleMesh.hxx"

#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <fstream>

TriangleMesh::TriangleMesh(const char* modelPath)
{
	loadOBJ(modelPath);
	computeNormals();
	m_modelMatrix = glm::mat4(1.0);
	
	update();
}

TriangleMesh::~TriangleMesh()
{
	glDeleteVertexArrays(1, &m_ID);
}

void TriangleMesh::update()
{
	glGenVertexArrays(1, &m_ID);
	glBindVertexArray(m_ID);

	m_vbo.setup(m_vertices.size() * sizeof(Vertex), m_vertices.data());
	// positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	// normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vertex::position)));
	glEnableVertexAttribArray(1);
	// texture coordinates
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vertex::position) + sizeof(Vertex::normal)));
	glEnableVertexAttribArray(2);
	//vertex colors
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vertex::position) + sizeof(Vertex::normal) + sizeof(Vertex::texcoord)));
	glEnableVertexAttribArray(3);

	m_ebo.setup(m_indices.size() * sizeof(Triangle), m_indices.data());
	m_numIndices = 3 * m_indices.size();

	glBindVertexArray(0);
}

void TriangleMesh::computeNormals()
{
	// compute normals
    for (Triangle& triangle : m_indices) {
        unsigned int idx0 = triangle[0], idx1 = triangle[1], idx2 = triangle[2];

        glm::vec3 vec1 = m_vertices[idx1].position - m_vertices[idx0].position,
                  vec2 = m_vertices[idx2].position - m_vertices[idx0].position, normal = glm::cross(vec1, vec2);
		
        m_vertices[idx0].normal += normal;
        m_vertices[idx1].normal += normal;
        m_vertices[idx2].normal += normal;
	}

	// normalize normals
	for (Vertex& vertex : m_vertices)
	{
		vertex.normal = glm::normalize(vertex.normal);
	}
}

void TriangleMesh::draw() const
{
	glBindVertexArray(m_ID);
	glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

glm::mat4 TriangleMesh::getModelMatrix() const
{
	return m_modelMatrix;
}

void TriangleMesh::rotate(glm::vec3 axis, float angle)
{
	glm::vec3 normAxis = glm::normalize(axis);
	m_modelMatrix = glm::rotate(m_modelMatrix, angle, normAxis);
}

void TriangleMesh::loadOBJ(const char* path)
{
	std::ifstream stream(path);
	if (!stream.is_open())
		std::cout << "ERROR::OBJ - failed to open .obj file" << std::endl;
	char first;
	std::string line;
	float x, y, z;
	unsigned int i, j, k;
	Vertex vertex;
	while (!stream.eof())
	{
		// get first char of a line and parse it
		stream >> first;
		switch (first)
		{
		case '#':
			std::getline(stream, line);
			break;

		case 'v':
			stream >> x >> y >> z;
			vertex.position = glm::vec3(x, y, z);
			m_vertices.push_back(vertex);
			break;

		case 'f':
			stream >> i >> j >> k;
			m_indices.push_back({ i - 1, j - 1, k - 1 });
			break;

		case 's':
			std::getline(stream, line);
			break;
		
		case 'o':
			std::getline(stream, line);
			break;
		
		case 'vt':
			std::getline(stream, line);
			break;
		
		case 'vn':
			std::getline(stream, line);
			break;

		default:
			std::cout << "ERROR::OBJ - failed to read .obj file [invalid token: '" << first << "']" << std::endl;
			return;
		}
	}
}
