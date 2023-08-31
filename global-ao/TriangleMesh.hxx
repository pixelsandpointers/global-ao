#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "BufferObject.hxx"

struct Vertex
{
	glm::vec3 position = glm::vec3(0.0);
	glm::vec3 normal = glm::vec3(0.0);
	glm::vec2 texcoord = glm::vec2(0.0);
	glm::vec4 color = glm::vec4(0.0);
};

using Triangle = glm::uvec3;

class TriangleMesh
{
private:
	unsigned int m_ID;

	VertexBuffer m_vbo;
	ElementBuffer m_ebo;
	std::vector<Vertex> m_vertices;
	std::vector<Triangle> m_indices;
	int m_numIndices;

	glm::mat4 m_modelMatrix;

public:
	TriangleMesh(const char* modelPath);
	~TriangleMesh();

	void draw() const;

	glm::mat4 getModelMatrix() const;

	std::vector<Vertex>* getVertices(){return &m_vertices;};
	std::vector<Triangle>* getIndices(){return &m_indices;};

	void setVertices(std::vector<Vertex>& vertices){
		m_vertices = vertices;
	};
	void setIndices(std::vector<Triangle>& indices){m_indices = indices; m_numIndices = indices.size();};

	void update();

	void rotate(glm::vec3 axis, float angle = 0.0005);

private:
	void loadOBJ(const char* path);
	void computeNormals();
};

