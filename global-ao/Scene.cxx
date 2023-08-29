#include "Scene.hxx"

Scene::Scene(std::vector<Model>& models) : m_models(models) {
    computeProjectionMatrix();
}

// draws the scene, and thus all its models
void Scene::Render(ShaderProgram& shader) const {
    shader.Use();
    shader.SetMat4("projectionMatrix", m_projectionMatrix);
    shader.SetMat4("viewMatrix", cam.GetViewMat());
    for (unsigned int i = 0; i < m_models.size(); i++) {
        m_models[i].Draw(shader);
    }
}

glm::mat4 Scene::GetProjectionMatrix() const {
    return m_projectionMatrix;
}

glm::vec3 Scene::GetBoundingCenter() const {
    return m_center;
}

float Scene::GetBoundingRadius() const {
    return m_radius;
}

std::vector<Model>& Scene::GetModels() {
    return m_models;
}

void Scene::ResetView() {
    glm::vec3 pos = GetBoundingCenter() - glm::vec3(0.0f, 0.0f, GetBoundingRadius());
    cam.SetView(pos, glm::vec3(0.0f, 0.0f, -1.0f));
}

void Scene::computeProjectionMatrix() {
    glm::vec3 c = glm::vec3(0.0f);
    float r = 0.0f;
    if (m_models.size() == 1) {
        c = m_models[0].GetBoundingCenter();
        r = m_models[0].GetBoundingRadius();
    } else {
        for (Model& model : m_models) {
            c += model.GetBoundingCenter();
            r = model.GetBoundingRadius();
        }
        c /= m_models.size();

        for (Model& model : m_models) {
            float dist = glm::length(c - model.GetBoundingCenter()) + model.GetBoundingRadius();
            if (dist > r)
                r = dist;
        }
    }
    m_center = c;
    m_radius = r;
    m_projectionMatrix = glm::ortho(-r, r, -r, r, 0.0f, 2.0f * r);
}