#include "Camera.hxx"
#include "DepthMap.hxx"
#include "Model.hxx"
#include "OcclusionMap.hxx"
#include "ShaderProgram.hxx"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <random>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, Camera* camera, Model* model) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

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
    // setup GLFW m_window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800, 800, "Global Ambient Occlusion", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "ERROR::GLFW - failed to create m_window\n" << std::endl;
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

    // texture rendering
    ShaderProgram textureShader("../../global-ao-testing/shader/texture.vert", "../../global-ao-testing/shader/texture.frag");
    Model quad("../../global-ao/resources/quad.txt");

    // rendering objects
    Camera camera(glm::vec3(0.0, 0.0, 1.0));
    Model model("../../global-ao/resources/backpack.obj");

    // MODE 0
    ShaderProgram basicShader("../../global-ao-testing/shader/basic.vert", "../../global-ao-testing/shader/basic.frag");

    // MODE 1
    ShaderProgram depthShader("../../global-ao-testing/shader/depth.vert", "../../global-ao-testing/shader/depth.frag");
    DepthMap depthMap;

    // MODE 2
    ShaderProgram uvShader("../../global-ao-testing/shader/uv.vert", "../../global-ao-testing/shader/uv.frag");

    // MODE 3
    ShaderProgram grayShader("../../global-ao-testing/shader/gray.vert", "../../global-ao-testing/shader/gray.frag");
    ShaderProgram accumShader("../../global-ao-testing/shader/accum.vert", "../../global-ao-testing/shader/accum.frag");
    OcclusionMap occlusionMap, occlusionMap1, occlusionMap2;

    // render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window, &camera, &model);

        int mode = 3;

        glm::mat4 modelMatrix = model.GetModelMatrix();
        //glm::mat4 modelMatrix = glm::scale(glm::translate(model.GetModelMatrix(), glm::vec3(0.0f, 0.0f, 0.0f)), glm::vec3(1.0f, 1.0f, 1.0f));
        glm::mat4 viewMatrix = camera.GetViewMatrix();
        //glm::mat4 projectionMatrix = glm::mat4(1.0f);
        glm::mat4 projectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
        //glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

        // basic render
        if (mode == 0) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            basicShader.Use();
            basicShader.SetMat4("modelMatrix", modelMatrix);
            basicShader.SetMat4("viewMatrix", viewMatrix);
            basicShader.SetMat4("projectionMatrix", projectionMatrix);

            model.Draw();
        }

        // render depth map
        if (mode == 1) {
            depthShader.Use();
            depthShader.SetMat4("modelMatrix", modelMatrix);
            depthShader.SetMat4("lightViewMatrix", viewMatrix);
            depthShader.SetMat4("projectionMatrix", projectionMatrix);
            depthMap.BindFramebuffer();
            model.Draw();
            depthMap.UnbindFramebuffer();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            textureShader.Use();
            textureShader.SetInt("colorTexture", 0);

            glActiveTexture(GL_TEXTURE0);
            depthMap.BindTexture();
            quad.Draw();
        }

        // render to uv-map
        if (mode == 2) {
            uvShader.Use();
            depthMap.BindFramebuffer();
            model.Draw();
            depthMap.UnbindFramebuffer();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            textureShader.Use();
            textureShader.SetInt("colorTexture", 0);

            glActiveTexture(GL_TEXTURE0);
            depthMap.BindTexture();
            quad.Draw();
        }

        // accumulate two framebuffer textures
        if (mode == 3) {
            grayShader.Use();
            grayShader.SetMat4("modelMatrix", modelMatrix);
            grayShader.SetMat4("viewMatrix", viewMatrix);
            grayShader.SetMat4("projectionMatrix", projectionMatrix);
            occlusionMap1.BindFramebuffer();
            model.Draw();
            occlusionMap1.UnbindFramebuffer();

            occlusionMap2.BindFramebuffer();
            model.Draw();
            occlusionMap2.UnbindFramebuffer();

            accumShader.Use();
            accumShader.SetMat4("modelMatrix", modelMatrix);
            accumShader.SetMat4("viewMatrix", viewMatrix);
            accumShader.SetMat4("projectionMatrix", projectionMatrix);
            accumShader.SetInt("tex1", 0);
            accumShader.SetInt("tex2", 1);
            glActiveTexture(GL_TEXTURE0);
            occlusionMap1.BindTexture();
            glActiveTexture(GL_TEXTURE1);
            occlusionMap2.BindTexture();
            occlusionMap.BindFramebuffer();
            model.Draw();
            occlusionMap.UnbindFramebuffer();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            textureShader.Use();
            textureShader.SetInt("colorTexture", 0);

            glActiveTexture(GL_TEXTURE0);
            occlusionMap.BindTexture();
            quad.Draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
