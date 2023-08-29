#pragma once

#include "Model.hxx"
#include "Camera.hxx"
#include "ShaderProgram.hxx"

#include <glm/glm.hpp>
#include <vector>

class Scene {
  public:
    Camera cam;

  private:
    std::vector<Model> m_models;

    glm::mat4 m_projectionMatrix = glm::mat4(1.0f);

    glm::vec3 m_center;
    float m_radius;


  public:
    Scene(std::vector<Model>& models);

    // draws the scene, and thus all its models
    void Render(ShaderProgram& shader) const;

    glm::mat4 GetProjectionMatrix() const;

    glm::vec3 GetBoundingCenter() const;

    float GetBoundingRadius() const;

    std::vector<Model>& GetModels();

    void ResetView();

  private:
    void computeProjectionMatrix();
};
