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

    glm::mat4 m_modelMatrix = glm::mat4(1.0f);

  public:
    // constructor, expects a filepath to a 3D model.
    Model(const std::string& path, bool gamma = false);

    glm::mat4 GetModelMatrix() const;;

    // draws the model, and thus all its meshes
    void Draw() const;

    void Rotate(glm::vec3 axis, float angle = 0.01);

  private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(const std::string& path);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};
