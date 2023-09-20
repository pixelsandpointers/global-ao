#include "GAOGenerator.hxx"

void GAOGenerator::computeOcclusion1(Scene& scene, int numLights, std::vector<OcclusionMap>& occlusionMaps) {
    // generate occlusion map per model
    std::vector<Model>& models = scene.GetModels();
    const unsigned int numModels = models.size();
    occlusionMaps.reserve(numModels);

    for (Model& model : models) {
        occlusionMaps.emplace_back();
    }

    // setup shaders, depthMap and constant uniforms
    glm::mat4 projectionMatrix = scene.GetProjectionMatrix();

    DepthMap depthMap;
    ShaderProgram depthShader("../../global-ao/shader/depth.vert", "../../global-ao/shader/depth.frag");

    ShaderProgram occlusionShader("../../global-ao/shader/occlusion.vert", "../../global-ao/shader/occlusion.frag");
    occlusionShader.Use();
    occlusionShader.SetInt("depthMap", 0);
    occlusionShader.SetMat4("projectionMatrix", projectionMatrix);
    occlusionShader.SetFloat("nSamples", numLights);
    occlusionShader.Unuse();

    Model quad("../../global-ao/resources/quad.obj");
    ShaderProgram textureShader("../../global-ao/shader/texture.vert", "../../global-ao/shader/texture.frag");

    // iterate over all lights
    for (unsigned int i = 0; i < numLights; i++) {
        // set random light position on bounding sphere of scene
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
            occlusionShader.SetMat4("viewMatrix", scene.cam.GetViewMatrix());

            glActiveTexture(GL_TEXTURE0);
            depthMap.BindTexture();

            occlusionMaps[i].BindFramebuffer();
            models[i].Draw();
            occlusionMaps[i].UnbindFramebuffer();
        }
        glDisable(GL_BLEND);
    }
    
    scene.ResetView();
}

void GAOGenerator::computeOcclusion2(Scene& scene, int numLights) {
    // generate attribute and occlusion map per model
    std::vector<Model>& models = scene.GetModels();
    const unsigned int numModels = models.size();
    std::vector<AttributeMap> positionMaps;
    positionMaps.reserve(numModels);
    std::vector<AttributeMap> normalMaps;
    normalMaps.reserve(numModels);
    std::vector<OcclusionMap> occlusionMaps;
    occlusionMaps.reserve(numModels);

    for (Model& model : models) {
        int tSize = model.GetTextureSize();

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

    // setup shaders, depthMap and constant uniforms
    glm::mat4 projectionMatrix = scene.GetProjectionMatrix();

    DepthMap depthMap;
    ShaderProgram depthShader("../../global-ao/shader/depth.vert", "../../global-ao/shader/depth.frag");

    ShaderProgram occlusionShader("../../global-ao/shader/occlusion2.vert", "../../global-ao/shader/occlusion2.frag");
    occlusionShader.Use();
    occlusionShader.SetInt("depthMap", 0);
    occlusionShader.SetInt("positionTex", 1);
    occlusionShader.SetInt("normalTex", 2);
    occlusionShader.SetMat4("projectionMatrix", projectionMatrix);
    occlusionShader.Unuse();

    Model quad("../../global-ao/resources/quad.obj");

    // iterate over all lights
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
            occlusionShader.SetFloat("viewportSize", models[i].GetTextureSize());
            occlusionShader.SetMat4("modelMatrix", models[i].GetModelMatrix());
            occlusionShader.SetMat4("viewMatrix", scene.cam.GetViewMatrix());
            occlusionShader.SetVec3("viewDir", scene.cam.GetViewDirection());

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
    
    // read data from occlusion texture per model
    for (int i = 0; i < models.size(); i++) {
        int tSize = models[i].GetTextureSize();
        
        glm::vec4* buffer = new glm::vec4[tSize * tSize];
        occlusionMaps[i].ReadData(buffer);

        // compute min and max occlusion value
        float min = (float)numLights;
        float max = 0.0f;
        for (int j = 0; j < models[i].GetNumVertices(); j++) {
            if (buffer[j].x > max)
                max = buffer[j].x;
            if (buffer[j].x < min)
                min = buffer[j].x;
        }

        // map occlusion values to [0.0, 1.0]
        for (int j = 0; j < models[i].GetNumVertices(); j++) {
            buffer[j] = (buffer[j] - min) / (max - min);
        }
        // update occlusion vertex attribute
        models[i].UpdateOcclusions(buffer);

        delete[] buffer;
    }
    quad.DeleteBuffers();

    scene.ResetView();
}

/*
void GAOGenerator::computeOcclusion1(
    Scene& const scene,
    Model& model,
    int numLights,
    OcclusionMap& const accumMap2,
    GLFWwindow* window) {

    ShaderProgram accumulationShader(
        "../../global-ao/shader/v3/accumulation3.vert", "../../global-ao/shader/v3/accumulation3.frag");
    
    const int numTextureUnits = 8;
    //glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &numTextureUnits);
    
    DepthMap depthMap[numTextureUnits];
    ShaderProgram depthShader("../../global-ao/shader/depth.vert", "../../global-ao/shader/depth.frag");

    ShaderProgram occlusionShader("../../global-ao/shader/v3/occlusion2.vert", "../../global-ao/shader/v3/occlusion2.frag");
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
