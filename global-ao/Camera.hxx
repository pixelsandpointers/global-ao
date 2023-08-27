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
    explicit Camera(
        glm::vec3 position = glm::vec3(0.0, 0.0, 1.0),
        glm::vec3 front = glm::vec3(0.0, 0.0, -1.0),
        glm::vec3 up = glm::vec3(0.0, 1.0, 0.0));;

    ~Camera() = default;

    glm::mat4 GetViewMatrix();

    void Move(glm::vec3 direction, float speed = 0.01);

    void Rotate(float angle, glm::vec3 axis);
};

