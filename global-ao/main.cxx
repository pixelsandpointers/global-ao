#include "Camera.hxx"
#include "DepthMap.hxx"
#include "Model.hxx"
#include "OcclusionMap.hxx"
#include "ShaderProgram.hxx"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <random>
#include <stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, Camera* camera, Model* model) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // move camera
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->Move(glm::vec3(-1.0, 0.0, 0.0));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->Move(glm::vec3(1.0, 0.0, 0.0));
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->Move(glm::vec3(0.0, 1.0, 0.0));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->Move(glm::vec3(0.0, -1.0, 0.0));
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera->Move(glm::vec3(0.0, 0.0, 1.0));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera->Move(glm::vec3(0.0, 0.0, -1.0));

    // rotate object
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        model->Rotate(glm::vec3(0.0, 1.0, 0.0));
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        model->Rotate(glm::vec3(0.0, -1.0, 0.0));
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        model->Rotate(glm::vec3(1.0, 0.0, 0.0));
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        model->Rotate(glm::vec3(-1.0, 0.0, 0.0));
}

int main() {
    // setup GLFW window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800, 800, "Global Ambient Occlusion", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "ERROR::GLFW - failed to create window\n" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // setup GLAD
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cout << "ERROR::GLAD - failed to initialize GLAD\n" << std::endl;
        return -1;
    }

    // setup viewport
    glViewport(0, 0, 800, 800);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // setup
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // AMBIENT OCCLUSION
    Model model("../../global-ao/resource/backpack.obj");
    ShaderProgram depthShader("../../global-ao/shader/depth.vert", "../../global-ao/shader/depth.frag");
    ShaderProgram occlusionShader("../../global-ao/shader/occlusion.vert", "../../global-ao/shader/occlusion.frag");
    ShaderProgram accumulationShader(
        "../../global-ao/shader/accumulation.vert", "../../global-ao/shader/accumulation.frag");
    DepthMap depthMap;
    OcclusionMap occlusionMap, accumMap1, accumMap2;
    unsigned int nLights = 1024;
    using Light = Camera;
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
        occlusionShader.SetFloat("nSamples", (float) nLights);
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

    // texture rendering
    ShaderProgram textureShader("../../global-ao/shader/texture.vert", "../../global-ao/shader/texture.frag");
    Model quad("../../global-ao/resource/quad.txt");

    // occluded object rendering
    ShaderProgram gaoShader("../../global-ao/shader/gao.vert", "../../global-ao/shader/gao.frag");
    Camera camera(glm::vec3(0.0, 0.0, 1.0));

    // render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window, &camera, &model);

        int mode = 1;

        // render texture (for debugging)
        if (mode == 0) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            textureShader.Use();
            textureShader.SetInt("colorTexture", 0);

            glActiveTexture(GL_TEXTURE0);
            accumMap2.BindTexture();
            quad.Draw();
        }

        // render model with occlusion texture
        if (mode == 1) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            gaoShader.Use();
            gaoShader.SetMat4("modelMatrix", model.GetModelMatrix());
            gaoShader.SetMat4("viewMatrix", camera.GetViewMatrix());
            gaoShader.SetMat4("projectionMatrix", projectionMatrix);
            gaoShader.SetInt("occlusionMap", 0);
            glActiveTexture(GL_TEXTURE0);
            accumMap2.BindTexture();
            model.Draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
