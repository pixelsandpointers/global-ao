#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
  private:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;

  public:
    /// Camera constructor
    /// \param position the position of the camera
    /// \param front focal length
    /// \param up translation around y-axis
    explicit Camera(
        glm::vec3 position = glm::vec3(0.0, 0.0, 0.0),
        glm::vec3 front = glm::vec3(0.0, 0.0, -1.0),
        glm::vec3 up = glm::vec3(0.0, 1.0, 0.0));

    ~Camera() = default;

    glm::vec3 GetViewDirection() const {
        return m_front;
    };
    /// computes the view matrix
    /// \return mat4 view matrix
    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    };

    void SetView(glm::vec3 position, glm::vec3 direction);
};
