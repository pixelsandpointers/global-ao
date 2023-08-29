#include "Camera.hxx"

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up)
  : m_position { position }, m_front { front }, m_up { up } {}

void Camera::ResetTransformation() {
    m_transformation = glm::mat4(1.0);
}

glm::mat4 Camera::GetViewMat() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::vec3 Camera::GetViewDir() {
    return m_front;
}

void Camera::Translate(glm::vec3 v) {
    m_transformation = glm::translate(m_transformation, v);
}

void Camera::Move(glm::vec3 direction, float speed) {
    glm::vec3 normDirection = glm::normalize(direction);
    m_position += normDirection * speed;
}

void Camera::Rotate(float angle, glm::vec3 axis) {
    glm::vec3 normAxis = glm::normalize(axis);
    m_transformation = glm::rotate(m_transformation, glm::radians(angle), normAxis);
}

void Camera::SetView(glm::vec3 pos, glm::vec3 dir) {
    m_position = pos;
    m_front = glm::normalize(dir);
    glm::vec3 right = glm::normalize(glm::cross(m_front, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_up = glm::normalize(glm::cross(right, m_front));
}


