#pragma once

#include "Mesh.hxx"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <fstream>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <stb_image.h>
#include <string>
#include <vector>

class Model {
  private:
    // model data
    std::vector<Mesh> m_meshes;
    std::string m_directory;
    bool m_gammaCorrection;
    unsigned int m_numVertices = 0;
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);

    // bounding sphere
    glm::vec3 m_center;
    float m_radius;

  public:
    // constructor, expects a filepath to a 3D model.
    Model(const std::string& path, bool gamma = false);

    ~Model() = default;

    /// delete all vertex array and vertex buffer objects
    void DeleteBuffers();

    std::vector<Mesh>& GetMeshes() {
        return m_meshes;
    };

    unsigned int GetNumVertices() const {
        return m_numVertices;
    };

    glm::mat4 GetModelMatrix() const {
        return m_modelMatrix;
    };

    glm::vec3 GetBoundingCenter() const {
        glm::vec4 c = m_modelMatrix * glm::vec4(m_center, 1.0);
        return glm::vec3(c.x, c.y, c.z) / c.w;
    };

    float GetBoundingRadius() const {
        return m_radius;
    };

    int GetTextureSize() const {
        return (int)ceil(sqrt((float)m_numVertices));
    };

    /// draws the model and its underlying meshes
    void Draw() const;

    /// draws the model and its underlying meshes (sets "modelMatrix")
    void Draw(ShaderProgram& shader) const;

    /// move the model by a translation vector
    /// \param translation vector
    void Move(glm::vec3 vector);

    /// move the model into direction by a distance
    /// \param direction vector
    /// \param distance
    void Move(glm::vec3 direction, float distance);

    /// rotate the model around an axis with a fixed angle
    /// \param axis to rotate model around
    /// \param angle the model should be rotated
    void Rotate(glm::vec3 axis, float angle = 0.01);

    /// scale the model by factors
    /// \param factors of scaling for each axis
    void Scale(glm::vec3 factors);

    /// updates the vertex occlusions and updates buffers
    /// \param pointer to data
    void UpdateOcclusions(glm::vec4* occlusions);

  private:
    /// updates the vertex occlusions and updates buffers!!!!!!!!!!!!!!!!!!!!!!!!!!
    void computeBoundingSphere();

    /// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    /// \param path to the model file
    void loadModel(const std::string& path);

    /// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    /// \param node Model node
    /// \param scene
    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};
