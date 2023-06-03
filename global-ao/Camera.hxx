#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
private:
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;

public:
	Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up) : m_position{ position }, m_front{ front }, m_up{ up } {};
	~Camera() = default;

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(m_position, m_position + m_front, m_up);
	}

	void move(glm::vec3 direction, float speed = 0.0005)
	{
		glm::vec3 normDirection = glm::normalize(direction);
		m_position += normDirection * speed;
	}

private:

};