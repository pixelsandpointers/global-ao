//
// Created by b on 6/21/23.
//

#ifndef GLOBAL_AO_MODEL_HXX
#define GLOBAL_AO_MODEL_HXX

#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.hxx"
#include "Mesh.hxx"

using std::map;
using std::string;
using std::vector;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model
{
  public:
    // model data
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model(string const &path, bool gamma = false);

    // draws the model, and thus all its meshes
    void Draw(Shader* shader)
    {
        for (auto & mesh : this->meshes)
            mesh.Draw(shader);
    }

  private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma);
#endif  //GLOBAL_AO_MODEL_HXX
