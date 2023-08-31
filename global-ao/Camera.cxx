#include "Camera.hxx"

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up) : m_position { position }, m_front { front }, m_up { up }
{
    m_right = glm::normalize(glm::cross(m_front, glm::vec3(0.0f, 1.0f, 0.0f)));
}

void Camera::SetView(glm::vec3 position, glm::vec3 direction) {
    m_position = position;
    m_front = glm::normalize(direction);
    m_right = glm::normalize(glm::cross(m_front, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}
