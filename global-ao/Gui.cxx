//
// Created by b on 6/21/23.
//

#include "Gui.hxx"

#include "AmbientOcclusion.hxx"
#include "Camera.hxx"
#include "Model.hxx"
#include "Shader.hxx"

//#include "Primitives.hxx"

Gui::Gui(int width, int height, const char* title) : windowWidth(width), windowHeight(height) {
    bool validCtx = this->glfwCreateContext(width, height, title);
}

Gui::~Gui() {
    glfwDestroyWindow(this->window);
}

void Gui::processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

bool Gui::glfwCreateContext(const int width, const int height, const char* windowTitle) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    this->window = glfwCreateWindow(width, height, windowTitle, nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create window.\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(this->window);

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cout << "Failed to init GLAD.\n";
        return EXIT_FAILURE;
    }

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(
        window, [](GLFWwindow* win, const int width, const int height) { glViewport(0, 0, width, height); });
    return true;
}

int Gui::run() {
    GLFWwindow* glfwCtx = glfwGetCurrentContext();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    AmbientOcclusion ao(this->windowWidth, this->windowHeight);
    Model model("../../resources/backpack/backpack.obj");
    Camera camera(glm::vec3(.0f, .0f, 5.f));
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.Zoom), static_cast<float>(this->windowWidth) / static_cast<float>(this->windowHeight), 0.1f,
        50.0f);

    // setup shaders and framebuffer
    ao.setShader(geometry, "ssaoGeometryVert.glsl", "ssaoGeometryFrag.glsl");
    ao.setShader(lighting, "ssaoVert.glsl", "ssaoLightingFrag.glsl");
    ao.setShader(texture, "ssaoVert.glsl", "ssaoFrag.glsl");
    ao.setShader(blur, "ssaoVert.glsl", "ssaoBlurFrag.glsl");

    ao.setupFrameBuffers();

    // shader config
    glm::vec3 lightPosition(2.0, 4.0, -2.0);
    glm::vec3 lightColor(.2, .2, .7);
    const std::pair<float, float> attenuation(0.09f, 0.032f);

    ao.getShaderPtr(lighting)->use();
    ao.getShaderPtr(lighting)->setInt("gPosition", 0);
    ao.getShaderPtr(lighting)->setInt("gNormal", 1);
    ao.getShaderPtr(lighting)->setInt("gAlbedo", 2);
    ao.getShaderPtr(lighting)->setInt("ssao", 3);
    ao.getShaderPtr(texture)->use();
    ao.getShaderPtr(texture)->setInt("gPosition", 0);
    ao.getShaderPtr(texture)->setInt("gNormal", 1);
    ao.getShaderPtr(texture)->setInt("texNoise", 2);
    ao.getShaderPtr(blur)->use();
    ao.getShaderPtr(blur)->setInt("ssaoInput", 0);

    // render loop
    float currentFrame;
    float deltaTime = .0f;
    float lastFrame = .0f;
    while (!glfwWindowShouldClose(glfwCtx)) {
        currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(glfwCtx);
        glClearColor(.0f, .0f, .0f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render passes
        ao.geometryPass(camera, model, projection);
        ao.texturePass(64, 16, projection);
        ao.blurPass();
        ao.lightingPass(camera, lightPosition, lightColor, attenuation);

        glfwSwapBuffers(glfwCtx);
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}
