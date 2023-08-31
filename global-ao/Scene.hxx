#pragma once

#include "Camera.hxx"
#include "Model.hxx"
#include "ShaderProgram.hxx"

#include <glm/glm.hpp>
#include <vector>

enum CameraMovement { TURN_LEFT, TURN_RIGHT, TURN_UP, TURN_DOWN };

class Scene {
  public:
    Camera cam;

  private:
    std::vector<Model> m_models;
    glm::mat4 m_projectionMatrix = glm::mat4(1.0f);

    // bounding sphere for all models
    glm::vec3 m_center;
    float m_radius;

    // camera parameters
    float m_yaw = -90.0f;
    float m_pitch = 0.0f;

  public:
    Scene(std::vector<Model>& models);

    ~Scene() = default;

    std::vector<Model>& GetModels() {
        return m_models;
    };

    glm::mat4 GetProjectionMatrix() const {
        return m_projectionMatrix;
    };

    glm::vec3 GetBoundingCenter() const {
        return m_center;
    };

    float GetBoundingRadius() const {
        return m_radius;
    };

    /// draws all models (sets "viewMatrix" and "projectionMatrix")
    void Render(ShaderProgram& shader) const;

    /// reset camera view to the border of the bounding sphere, looking at -z
    void ResetView();

    /// processes input received from a keyboard input
    void ProcessKeyboard(CameraMovement movement, float deltaTime);

  private:
    void computeProjectionMatrix();
    void updateCamera();
};
