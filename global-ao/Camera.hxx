#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
  private:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;

    glm::mat4 m_transformation = glm::mat4(1.0);

  public:
    Camera(
        glm::vec3 position = glm::vec3(0.0, 0.0, 1.0),
        glm::vec3 front = glm::vec3(0.0, 0.0, -1.0),
        glm::vec3 up = glm::vec3(0.0, 1.0, 0.0))
      : m_position { position }, m_front { front }, m_up { up } {};

    ~Camera() = default;

    glm::mat4 GetViewMatrix() {
        return m_transformation * glm::lookAt(m_position, m_position + m_front, m_up);
    }

    void Move(glm::vec3 direction, float speed = 0.0005) {
        glm::vec3 normDirection = glm::normalize(direction);
        m_position += normDirection * speed;
    }

    void Rotate(float angle, glm::vec3 axis) {
        m_transformation = glm::rotate(m_transformation, glm::radians(angle), axis);
    }
};