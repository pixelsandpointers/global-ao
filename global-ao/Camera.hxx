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
    /// Camera constructor
    /// \param position the position of the camera
    /// \param front focal length
    /// \param up translation around y-axis
    explicit Camera(
        glm::vec3 position = glm::vec3(0.0, 0.0, 0.0),
        glm::vec3 front = glm::vec3(0.0, 0.0, -1.0),
        glm::vec3 up = glm::vec3(0.0, 1.0, 0.0));

    ~Camera() = default;

    /// Resets current camera transformation to identity matrix
    void ResetTransformation();

    /// Computes the view matrix
    /// \return mat4 view matrix
    glm::mat4 GetViewMat() const;

    /// Computes view direction
    /// \return vec3 view direction
    glm::vec3 GetViewDir();

    void Translate(glm::vec3 v);

    /// Move the camera to position
    /// \param direction directional vector in which we want to translate to
    /// \param speed the acceleration of the translation
    void Move(glm::vec3 direction, float speed = 0.01);

    /// Rotate the camera around an axis given an angle
    /// \param angle angle to rotate
    /// \param axis rotate along this axis
    void Rotate(float angle, glm::vec3 axis);

    void SetView(glm::vec3 pos, glm::vec3 dir);
};

