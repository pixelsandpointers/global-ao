#include "Scene.hxx"

Scene::Scene(std::vector<Model>& models) : m_models(models) {
    computeProjectionMatrix();
    ResetView();
}

void Scene::Render(ShaderProgram& shader) const {
    shader.Use();
    shader.SetMat4("projectionMatrix", m_projectionMatrix);
    shader.SetMat4("viewMatrix", cam.GetViewMatrix());
    for (unsigned int i = 0; i < m_models.size(); i++) {
        m_models[i].Draw(shader);
    }
}

void Scene::ResetView() {
    m_yaw = -90.0f;
    m_pitch = 0.0f;
    updateCamera();
}

void Scene::computeProjectionMatrix() {
    glm::vec3 center = glm::vec3(0.0f);
    float radius = 0.0f;
    for (Model& model : m_models) {
        center += model.GetBoundingCenter();
        radius = model.GetBoundingRadius();
    }
    center /= m_models.size();

    for (Model& model : m_models) {
        float dist = glm::length(center - model.GetBoundingCenter()) + model.GetBoundingRadius();
        if (dist > radius)
            radius = dist;
    }
    m_center = center;
    m_radius = radius;
    m_projectionMatrix = glm::ortho(-radius, radius, -radius, radius, 0.0f, 2.0f * radius);
}

void Scene::ProcessKeyboard(CameraMovement movement, float deltaTime) {
    float turnSpeed = 60.0f;
    float turnVelocity = turnSpeed * deltaTime;
    if (movement == TURN_LEFT)
        m_yaw += turnVelocity;
    if (movement == TURN_RIGHT)
        m_yaw -= turnVelocity;
    if (movement == TURN_UP)
        m_pitch -= turnVelocity;
    if (movement == TURN_DOWN)
        m_pitch += turnVelocity;

    if (m_pitch > 89.0f)
        m_pitch = 89.0f;
    if (m_pitch < -89.0f)
        m_pitch = -89.0f;

    updateCamera();
}

void Scene::updateCamera() {
    glm::vec3 dir;
    dir.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    dir.y = sin(glm::radians(m_pitch));
    dir.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    dir = glm::normalize(dir);
    glm::vec3 pos = m_center - m_radius * dir;
    cam.SetView(pos, dir);
}