//
// Created by b on 6/21/23.
//

#include "AmbientOcclusion.hxx"
#include "Gui.hxx"
#include "Camera.hxx"
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
    glfwSetFramebufferSizeCallback(window, [] (GLFWwindow* win, const int width, const int height) {
        glViewport(0, 0, width, height);
    });
    return true;
}

int Gui::run() {
    GLFWwindow* glfwCtx = glfwGetCurrentContext();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // setup area
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    // - setup end

    // we switch the directory to the shader dir within the shader class
    AmbientOcclusion ao(this->windowWidth, this->windowHeight);
    // Shader shader("basicVert.glsl", "basicFrag.glsl");
    ao.setShader(other, "basicVert.glsl", "basicFrag.glsl");
    Camera camera(glm::vec3(0.0, 0.0, 1.0),
                  glm::vec3(0.0, 0.0, -1.0),
                  glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.Zoom), static_cast<float>(this->windowWidth) / static_cast<float>(this->windowHeight), 0.1f,
        50.0f);

    // render loop
    while (!glfwWindowShouldClose(glfwCtx)) {
        processInput(glfwCtx);
        glClearColor(.2f, .3f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ao.getShaderPtr(other)->use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(glfwCtx);
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}
