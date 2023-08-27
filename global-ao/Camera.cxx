#include "Camera.hxx"

void Camera::Move(glm::vec3 direction, float speed) {
    glm::vec3 normDirection = glm::normalize(direction);
    m_position += normDirection * speed;
}

void Camera::Rotate(float angle, glm::vec3 axis) {
    m_transformation = glm::rotate(m_transformation, glm::radians(angle), axis);
}

glm::mat4 Camera::GetViewMatrix() {
    return m_transformation * glm::lookAt(m_position, m_position + m_front, m_up);
}

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up)
    : m_position { position }, m_front { front }, m_up { up } {}
