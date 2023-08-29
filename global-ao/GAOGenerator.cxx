#include "GAOGenerator.hxx"

using Light = Camera;

void GAOGenerator::computeOcclusion0(Scene& scene, int numLights) {
    glm::vec2 texSize(512.0f, 512.0f);
    
    std::vector<Model>& models = scene.GetModels();
    const unsigned int numModels = models.size();
    std::vector<AttributeMap> positionMaps;
    positionMaps.reserve(numModels);
    std::vector<AttributeMap> normalMaps;
    normalMaps.reserve(numModels);
    std::vector<OcclusionMap> occlusionMaps;
    occlusionMaps.reserve(numModels);

    for (Model& model : models) {
        // compute min texsize
        unsigned int tSize = 512;
        //while (tSize)

        glm::vec4* vertexPositions = new glm::vec4[tSize * tSize];
        glm::vec4* vertexNormals = new glm::vec4[tSize * tSize];
        unsigned int i = 0;
        for (Mesh& mesh : model.GetMeshes()) {
            for (Vertex& vertex : mesh.GetVertices()) {
                vertexPositions[i] = glm::vec4(vertex.Position, 1.0f);
                vertexNormals[i] = glm::vec4(vertex.Normal, 1.0f);
                i++;
            }
        }
        positionMaps.emplace_back(tSize, tSize, vertexPositions);
        normalMaps.emplace_back(tSize, tSize, vertexNormals);
        occlusionMaps.emplace_back(tSize, tSize);

        delete[] vertexPositions;
        delete[] vertexNormals;
    }

    glm::mat4 projectionMatrix = scene.GetProjectionMatrix();

    DepthMap depthMap;
    ShaderProgram depthShader("../../global-ao/shader/depth.vert", "../../global-ao/shader/depth.frag");

    ShaderProgram occlusionShader("../../global-ao/shader/v0/occlusion.vert", "../../global-ao/shader/v0/occlusion.frag");
    occlusionShader.Use();
    occlusionShader.SetInt("depthMap", 0);
    occlusionShader.SetInt("positionTex", 1);
    occlusionShader.SetInt("normalTex", 2);
    occlusionShader.SetMat4("projectionMatrix", projectionMatrix);
    occlusionShader.SetVec2("viewportSize", texSize);
    occlusionShader.Unuse();

    Model quad("../../global-ao/resources/quad.obj");

    for (unsigned int i = 0; i < numLights; i++) {
        float rx = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f;
        float ry = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f;
        float rz = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f;
        glm::vec3 randomDir = glm::normalize(glm::vec3(rx, ry, rz));
        glm::vec3 randomPos = scene.GetBoundingRadius() * randomDir + scene.GetBoundingCenter();
        scene.cam.SetView(randomPos, -randomDir);

        // depth pass
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        depthMap.BindFramebuffer();
        scene.Render(depthShader);
        depthMap.UnbindFramebuffer();
        glDisable(GL_DEPTH_TEST);

        // texture pass
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        occlusionShader.Use();
        for (int i = 0; i < models.size(); i++) {
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            occlusionShader.SetMat4("modelMatrix", models[i].GetModelMatrix());
            occlusionShader.SetMat4("viewMatrix", scene.cam.GetViewMat());
            occlusionShader.SetVec3("viewDir", scene.cam.GetViewDir());

            glActiveTexture(GL_TEXTURE0);
            depthMap.BindTexture();
            glActiveTexture(GL_TEXTURE1);
            positionMaps[i].BindTexture();
            glActiveTexture(GL_TEXTURE2);
            normalMaps[i].BindTexture();

            occlusionMaps[i].BindFramebuffer();
            quad.Draw();
            occlusionMaps[i].UnbindFramebuffer();
        }
        glDisable(GL_BLEND);
    }
    
    for (int i = 0; i < models.size(); i++) {
        int tSize = 1900;
        
        glm::vec4* buffer = new glm::vec4[tSize * tSize];
        occlusionMaps[i].ReadData(buffer);

        float min = (float)numLights;
        float max = 0.0f;
        for (int j = 0; j < models[i].GetNumVertices(); j++) {
            if (buffer[j].x > max)
                max = buffer[j].x;
            if (buffer[j].x < min)
                min = buffer[j].x;
        }

        for (int j = 0; j < models[i].GetNumVertices(); j++) {
            buffer[j] = (buffer[j] - min) / (max - min);
        }
        models[i].UpdateColors(buffer);

        delete[] buffer;
    }
    quad.DeleteBuffers();

    //scene.ResetView();
}


void GAOGenerator::computeOcclusion1(Scene& const scene, int numLights, std::vector<OcclusionMap>& occlusionMaps) {
    std::vector<Model>& models = scene.GetModels();
    const unsigned int numModels = models.size();
    occlusionMaps.reserve(numModels);

    for (Model& model : models) {
        occlusionMaps.emplace_back();
    }

    glm::mat4 projectionMatrix = scene.GetProjectionMatrix();
    /*
    std::vector<DepthMap> depthMaps;
    depthMaps.reserve(numD);
    */
    DepthMap depthMap;
    ShaderProgram depthShader("../../global-ao/shader/depth.vert", "../../global-ao/shader/depth.frag");

    ShaderProgram occlusionShader("../../global-ao/shader/v1/occlusion.vert", "../../global-ao/shader/v1/occlusion.frag");
    occlusionShader.Use();
    occlusionShader.SetInt("depthMap", 0);
    occlusionShader.SetMat4("projectionMatrix", projectionMatrix);
    occlusionShader.SetFloat("nSamples", numLights);
    occlusionShader.Unuse();

    Model quad("../../global-ao/resources/quad.obj");
    ShaderProgram textureShader("../../global-ao/shader/texture.vert", "../../global-ao/shader/texture.frag");

    for (unsigned int i = 0; i < numLights; i++) {
        float rx = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f;
        float ry = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f;
        float rz = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f;
        glm::vec3 randomDir = glm::normalize(glm::vec3(rx, ry, rz));
        glm::vec3 randomPos = scene.GetBoundingRadius() * randomDir + scene.GetBoundingCenter();
        scene.cam.SetView(randomPos, -randomDir);

        // depth pass
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        depthMap.BindFramebuffer();
        scene.Render(depthShader);
        depthMap.UnbindFramebuffer();
        glDisable(GL_DEPTH_TEST);

        // texture pass
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        occlusionShader.Use();
        for (int i = 0; i < models.size(); i++) {
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            occlusionShader.SetMat4("modelMatrix", models[i].GetModelMatrix());
            occlusionShader.SetMat4("viewMatrix", scene.cam.GetViewMat());
            occlusionShader.SetVec3("viewDir", scene.cam.GetViewDir());

            glActiveTexture(GL_TEXTURE0);
            depthMap.BindTexture();

            occlusionMaps[i].BindFramebuffer();
            models[i].Draw();
            occlusionMaps[i].UnbindFramebuffer();
        }
        glDisable(GL_BLEND);
    }
    //scene.ResetView();
}

/*
void GAOGenerator::computeOcclusion1(
    Scene& const scene,
    Model& model,
    int numLights,
    OcclusionMap& const accumMap2,
    GLFWwindow* window) {

    ShaderProgram accumulationShader(
        "../../global-ao/shader/v3/accumulation.vert", "../../global-ao/shader/v3/accumulation.frag");
    
    const int numTextureUnits = 8;
    //glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &numTextureUnits);
    
    DepthMap depthMap[numTextureUnits];
    ShaderProgram depthShader("../../global-ao/shader/depth.vert", "../../global-ao/shader/depth.frag");

    ShaderProgram occlusionShader("../../global-ao/shader/v3/occlusion.vert", "../../global-ao/shader/v3/occlusion.frag");
    occlusionShader.Use();
    occlusionShader.SetInt("depthMap", 0);
    occlusionShader.SetFloat("nSamples", static_cast<float>(numLights));
    occlusionShader.Unuse();

    OcclusionMap occlusionMap[8], accumMap1;  //, accumMap2;
    Light light;

    glm::mat4 modelMatrix = model.GetModelMatrix();


    Model quad("../../global-ao/resources/quad.obj");
    ShaderProgram textureShader("../../global-ao/shader/texture.vert", "../../global-ao/shader/texture.frag");

    int j = 0;
    for (unsigned int i = 0; i < numLights; i++) {
        float rx = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f;
        float ry = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f;
        float rz = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f;
        glm::vec3 randomDir = glm::normalize(glm::vec3(rx, ry, rz));
        glm::vec3 randomPos = scene.GetBoundingRadius() * randomDir + scene.GetBoundingCenter();
        scene.cam.SetView(randomPos, -randomDir);

        // depth pass
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        depthMap.BindFramebuffer();
        scene.Render(depthShader);
        depthMap.UnbindFramebuffer();
        glDisable(GL_DEPTH_TEST);

        // texture pass
        occlusionShader.Use();
        occlusionShader.SetMat4("modelMatrix", modelMatrix);
        occlusionShader.SetMat4("lightViewMatrix", scene.cam.GetViewMat());
        occlusionShader.SetMat4("projectionMatrix", scene.GetProjectionMatrix());
        glActiveTexture(GL_TEXTURE0);
        depthMap.BindTexture();
        occlusionMap[i % 8].BindFramebuffer();
        model.Draw();
        occlusionMap[i % 8].UnbindFramebuffer();

        // accumulation
        if (i % 8 == 7) {
            accumulationShader.Use();
            if (j % 2 == 0) {
                accumulationShader.SetInt("currentMap", 0);
                accumulationShader.SetInt("newMap1", 1);
                accumulationShader.SetInt("newMap2", 2);
                accumulationShader.SetInt("newMap3", 3);
                accumulationShader.SetInt("newMap4", 4);
                accumulationShader.SetInt("newMap5", 5);
                accumulationShader.SetInt("newMap6", 6);
                accumulationShader.SetInt("newMap7", 7);
                accumulationShader.SetInt("newMap8", 8);
                glActiveTexture(GL_TEXTURE0);
                accumMap1.BindTexture();
                glActiveTexture(GL_TEXTURE1);
                occlusionMap[0].BindTexture();
                glActiveTexture(GL_TEXTURE2);
                occlusionMap[1].BindTexture();
                glActiveTexture(GL_TEXTURE3);
                occlusionMap[2].BindTexture();
                glActiveTexture(GL_TEXTURE4);
                occlusionMap[3].BindTexture();
                glActiveTexture(GL_TEXTURE5);
                occlusionMap[4].BindTexture();
                glActiveTexture(GL_TEXTURE6);
                occlusionMap[5].BindTexture();
                glActiveTexture(GL_TEXTURE7);
                occlusionMap[6].BindTexture();
                glActiveTexture(GL_TEXTURE8);
                occlusionMap[7].BindTexture();

                accumMap2.BindFramebuffer();
                model.Draw();
                accumMap2.UnbindFramebuffer();
            } else {
                accumulationShader.SetInt("currentMap", 0);
                accumulationShader.SetInt("newMap1", 1);
                accumulationShader.SetInt("newMap2", 2);
                accumulationShader.SetInt("newMap3", 3);
                accumulationShader.SetInt("newMap4", 4);
                accumulationShader.SetInt("newMap5", 5);
                accumulationShader.SetInt("newMap6", 6);
                accumulationShader.SetInt("newMap7", 7);
                accumulationShader.SetInt("newMap8", 8);
                glActiveTexture(GL_TEXTURE0);
                accumMap2.BindTexture();
                glActiveTexture(GL_TEXTURE1);
                occlusionMap[0].BindTexture();
                glActiveTexture(GL_TEXTURE2);
                occlusionMap[1].BindTexture();
                glActiveTexture(GL_TEXTURE3);
                occlusionMap[2].BindTexture();
                glActiveTexture(GL_TEXTURE4);
                occlusionMap[3].BindTexture();
                glActiveTexture(GL_TEXTURE5);
                occlusionMap[4].BindTexture();
                glActiveTexture(GL_TEXTURE6);
                occlusionMap[5].BindTexture();
                glActiveTexture(GL_TEXTURE7);
                occlusionMap[6].BindTexture();
                glActiveTexture(GL_TEXTURE8);
                occlusionMap[7].BindTexture();

                accumMap1.BindFramebuffer();
                model.Draw();
                accumMap1.UnbindFramebuffer();
            }
            j++;
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        textureShader.Use();
        textureShader.SetInt("colorTexture", 0);

        glActiveTexture(GL_TEXTURE0);
        depthMap.BindTexture();
        quad.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
*/


/*
// backup version 0 for single model
void GAOGenerator::computeOcclusion4(
    Scene& const scene,
    Model& model,
    int nLights,
    OcclusionMap& const accumMap2,
    GLFWwindow* window) {
    std::vector<Model> models = scene.GetModels();

    float* vertexPositions = new GLfloat[2048 * 2048 * 4];
    GLfloat* vertexNormals = new GLfloat[2048 * 2048 * 4];


    unsigned int i = 0;
    for (Mesh& mesh : model.GetMeshes()) {
        for (Vertex& vert : mesh.GetVertices()) {
            // vertex position
            vertexPositions[i + 0] = vert.Position.x;
            vertexPositions[i + 1] = vert.Position.y;
            vertexPositions[i + 2] = vert.Position.z;
            vertexPositions[i + 3] = 1.0;

            // normal vector
            vertexNormals[i + 0] = vert.Normal.x;
            vertexNormals[i + 1] = vert.Normal.y;
            vertexNormals[i + 2] = vert.Normal.z;
            vertexNormals[i + 3] = 1.0;

            i += 4;
        }
    }
    unsigned int textureSize = 2048;
    AttributeMap vertexPositionTex(textureSize, textureSize, vertexPositions);
    AttributeMap vertexNormalTex(textureSize, textureSize, vertexNormals);

    glm::mat4 modelMatrix = model.GetModelMatrix();
    glm::mat4 projectionMatrix = scene.GetProjectionMatrix();

    DepthMap depthMap;
    ShaderProgram depthShader("../../global-ao/shader/depth.vert", "../../global-ao/shader/depth.frag");

    OcclusionMap occlusionTex(textureSize, textureSize);
    ShaderProgram occlusionShader(
        "../../global-ao/shader/v0/occlusion.vert", "../../global-ao/shader/v0/occlusion.frag");
    occlusionShader.Use();
    occlusionShader.SetInt("depthMap", 0);
    occlusionShader.SetInt("positionTex", 1);
    occlusionShader.SetInt("normalTex", 2);
    occlusionShader.SetMat4("projectionMatrix", projectionMatrix);
    occlusionShader.Unuse();

    Light light;

    Model quad("../../global-ao/resources/quad.obj");


    for (unsigned int i = 0; i < 1024; i++) {
        scene.cam.ResetTransformation();
        float rx = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 360.0f;
        float ry = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 360.0f;
        float rz = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 360.0f;
        scene.cam.Rotate(rx, { 1.0, 0.0, 0.0 });
        scene.cam.Rotate(ry, { 0.0, 1.0, 0.0 });
        scene.cam.Rotate(rz, { 0.0, 0.0, 1.0 });

        glm::mat4 viewMatrix = scene.cam.GetViewMat();
        glm::vec3 viewDir = scene.cam.GetViewDir();

        // depth pass
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        depthMap.BindFramebuffer();
        scene.Render(depthShader);
        depthMap.UnbindFramebuffer();
        glDisable(GL_DEPTH_TEST);

        // texture pass
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glClear(GL_DEPTH_BUFFER_BIT);
        occlusionShader.Use();
        for (int i = 0; i < models.size(); i++) {
            occlusionShader.SetMat4("modelMatrix", models[i].GetModelMatrix());
            occlusionShader.SetMat4("viewMatrix", viewMatrix);
            occlusionShader.SetVec3("viewDir", viewDir);

            glActiveTexture(GL_TEXTURE0);
            depthMap.BindTexture();
            glActiveTexture(GL_TEXTURE1);
            vertexPositionTex.BindTexture();
            glActiveTexture(GL_TEXTURE2);
            vertexNormalTex.BindTexture();

            occlusionTex.BindFramebuffer();
            quad.Draw();
            occlusionTex.UnbindFramebuffer();
        }
        glDisable(GL_BLEND);
    }

    float* result = new float[2048 * 2048 * 4];
    occlusionTex.ReadData(result);

    float min = 1000.0f;
    float max = 0.0f;
    for (int i = 0; i < model.GetNumVertices(); i++) {
        int idx = i * 4;
        if (result[idx] > max)
            max = result[idx];
        if (result[idx] < min)
            min = result[idx];
    }


    std::vector<glm::vec4> occlusion;
    occlusion.reserve(model.GetNumVertices());
    for (int i = 0; i < model.GetNumVertices(); i++) {
        int idx = i * 4;
        glm::vec4 color(result[idx], result[idx + 1], result[idx + 2], result[idx + 3]);
        occlusion.push_back((color - min) / (max - min));
    }
    //model.UpdateColors(buffer);

    delete[] result;
}
*/

/*
// backup version 1 for single model
void GAOGenerator::computeOcclusion1(
    const Scene& const scene,
    const Model& const model,
    int nLights,
    OcclusionMap& const accumMap2,
    GLFWwindow* window) {
    ShaderProgram depthShader("../../global-ao/shader/depth.vert", "../../global-ao/shader/depth.frag");
    ShaderProgram occlusionShader(
        "../../global-ao/shader/v1/occlusion.vert", "../../global-ao/shader/v1/occlusion.frag");
    ShaderProgram accumulationShader(
        "../../global-ao/shader/v1/accumulation.vert", "../../global-ao/shader/v1/accumulation.frag");

    DepthMap depthMap;
    OcclusionMap occlusionMap, accumMap1;  //, accumMap2;
    Light light(model.GetBoundingCenter());

    Model quad("../../global-ao/resources/quad.obj");
    ShaderProgram textureShader("../../global-ao/shader/texture.vert", "../../global-ao/shader/texture.frag");

    // set model and projection matrix (whole model should be visible)
    // TODO: change shaders so projection matrices also work
    glm::mat4 modelMatrix = model.GetModelMatrix();
    glm::mat4 projectionMatrix = scene.GetProjectionMatrix();

    for (unsigned int i = 0; i < nLights; i++) {
        light.ResetTransformation();
        float rx = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 360.0f;
        float ry = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 360.0f;
        float rz = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 360.0f;
        light.Rotate(rx, { 1.0, 0.0, 0.0 });
        light.Rotate(ry, { 0.0, 1.0, 0.0 });
        light.Rotate(rz, { 0.0, 0.0, 1.0 });

        glm::mat4 viewMatrix = light.GetViewMat();

        // depth pass
        glEnable(GL_DEPTH_TEST);
        depthShader.Use();
        depthShader.SetMat4("modelMatrix", modelMatrix);
        depthShader.SetMat4("lightViewMatrix", viewMatrix);
        depthShader.SetMat4("projectionMatrix", projectionMatrix);
        depthMap.BindFramebuffer();
        model.Draw();
        depthMap.UnbindFramebuffer();

        // texture pass
        occlusionShader.Use();
        occlusionShader.SetMat4("modelMatrix", modelMatrix);
        occlusionShader.SetMat4("lightViewMatrix", viewMatrix);
        occlusionShader.SetMat4("projectionMatrix", projectionMatrix);
        occlusionShader.SetInt("depthMap", 0);
        occlusionShader.SetFloat("nSamples", static_cast<float>(nLights));
        glActiveTexture(GL_TEXTURE0);
        depthMap.BindTexture();
        occlusionMap.BindFramebuffer();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        model.Draw();
        occlusionMap.UnbindFramebuffer();

        // accumulation
        accumulationShader.Use();
        if (i % 2 == 0) {
            accumulationShader.SetInt("currentMap", 0);
            accumulationShader.SetInt("newMap", 1);
            glActiveTexture(GL_TEXTURE0);
            accumMap1.BindTexture();
            glActiveTexture(GL_TEXTURE1);
            occlusionMap.BindTexture();

            accumMap2.BindFramebuffer();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            model.Draw();
            accumMap2.UnbindFramebuffer();
        } else {
            accumulationShader.SetInt("currentMap", 0);
            accumulationShader.SetInt("newMap", 1);
            glActiveTexture(GL_TEXTURE0);
            accumMap2.BindTexture();
            glActiveTexture(GL_TEXTURE1);
            occlusionMap.BindTexture();

            accumMap1.BindFramebuffer();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            model.Draw();
            accumMap1.UnbindFramebuffer();
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        textureShader.Use();
        textureShader.SetInt("colorTexture", 0);

        glActiveTexture(GL_TEXTURE0);
        accumMap2.BindTexture();
        quad.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
        //_sleep(500);
    }
}
*/