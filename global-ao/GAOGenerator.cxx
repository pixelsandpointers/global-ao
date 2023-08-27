#include "GAOGenerator.hxx"

void GAOGenerator::ComputeOcclusion(Model& model, int nLights, OcclusionMap& accumMap2) {
    ShaderProgram depthShader("../../global-ao/shader/depth.vert", "../../global-ao/shader/depth.frag");
    ShaderProgram occlusionShader("../../global-ao/shader/v1/occlusion.vert", "../../global-ao/shader/v1/occlusion.frag");
    ShaderProgram accumulationShader("../../global-ao/shader/v1/accumulation.vert", "../../global-ao/shader/v1/accumulation.frag");

    DepthMap depthMap;
    OcclusionMap occlusionMap, accumMap1;  //, accumMap2;
    Light light;

    // set model and projection matrix (whole model should be visible)
    // TODO: change shaders so projection matrices also work
    glm::mat4 modelMatrix = model.GetModelMatrix();
    glm::mat4 projectionMatrix = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -10.0f, 10.0f);

    for (unsigned int i = 0; i < nLights; i++) {
        float rx = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 360.0f;
        float ry = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 360.0f;
        float rz = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 360.0f;
        light.Rotate(rx, glm::vec3(1.0, 0.0, 0.0));
        light.Rotate(ry, glm::vec3(0.0, 1.0, 0.0));
        light.Rotate(rz, glm::vec3(0.0, 0.0, 1.0));

        glm::mat4 viewMatrix = light.GetViewMatrix();

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
            model.Draw();
            accumMap1.UnbindFramebuffer();
        }
    }
}
