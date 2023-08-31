#include "Model.hxx"

Model::Model(const std::string& path, bool gamma) : m_gammaCorrection(gamma) {
    loadModel(path);
    computeBoundingSphere();
    Move(-m_center);
}

void Model::DeleteBuffers() {
    for (Mesh& mesh : m_meshes) {
        glDeleteBuffers(1, &mesh.m_VBO);
        glDeleteBuffers(1, &mesh.m_EBO);
        glDeleteVertexArrays(1, &mesh.m_VAO);
    }
}

void Model::Draw() const {
    for (unsigned int i = 0; i < m_meshes.size(); i++)
        m_meshes[i].Draw();
}

void Model::Draw(ShaderProgram& shader) const {
    shader.SetMat4("modelMatrix", m_modelMatrix);
    for (unsigned int i = 0; i < m_meshes.size(); i++)
        m_meshes[i].Draw();
}

void Model::Move(glm::vec3 vector) {
    m_modelMatrix = glm::translate(m_modelMatrix, vector);
}

void Model::Move(glm::vec3 direction, float distance) {
    glm::vec3 normDirection = glm::normalize(direction);
    m_modelMatrix = glm::translate(m_modelMatrix, distance * normDirection);
}

void Model::Rotate(glm::vec3 axis, float angle) {
    glm::vec3 normAxis = glm::normalize(axis);
    m_modelMatrix = glm::translate(m_modelMatrix, m_center);
    m_modelMatrix = glm::rotate(m_modelMatrix, angle, normAxis);
    m_modelMatrix = glm::translate(m_modelMatrix, -m_center);
}

void Model::Scale(glm::vec3 factors) {
    m_modelMatrix = glm::scale(m_modelMatrix, factors);
    float max = factors.x;
    if (factors.y > max)
        max = factors.y;
    if (factors.z > max)
        max = factors.z;
    m_radius *= max;
}

void Model::UpdateOcclusions(glm::vec4* occlusions) {
    int i = 0;
    for (Mesh& mesh : m_meshes) {
        for (Vertex& vertex : mesh.GetVertices()) {
            vertex.Occlusion = occlusions[i];
            i++;
        }
        mesh.UpdateBuffers();
    }
}

void Model::computeBoundingSphere() {
    glm::vec3 min(0.0f);
    glm::vec3 max(0.0f);
    for (Mesh& mesh : m_meshes) {
        for (Vertex& vertex : mesh.GetVertices()) {
            glm::vec3 pos = vertex.Position;
            if (pos.x < min.x)
                min.x = pos.x;
            if (pos.x > max.x)
                max.x = pos.x;
            if (pos.y < min.y)
                min.y = pos.y;
            if (pos.y > max.y)
                max.y = pos.y;
            if (pos.z < min.z)
                min.z = pos.z;
            if (pos.z > max.z)
                max.z = pos.z;
        }
    }
    m_center = 0.5f * (min + max);
    m_radius = glm::length(max - m_center);
}

void Model::loadModel(const std::string& path) {
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)  // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP - " << importer.GetErrorString() << std::endl;
        return;
    }
    // retrieve the directory path of the filepath
    m_directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    m_numVertices += mesh->mNumVertices;
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector;
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0])  // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.Texcoord = vec;
            /* ignore tangents for now
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
            */
        } else
            vertex.Texcoord = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // walk through each of the mesh's faces
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    /* ignore materials for now
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    */

    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices);
}
