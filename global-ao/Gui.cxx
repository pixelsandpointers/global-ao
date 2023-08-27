//
// Created by b on 20.08.23.
//

#include "Gui.hxx"

Gui::Gui(unsigned int windowWidth, unsigned int windowHeight) {
    this->m_windowWidth = windowWidth;
    this->m_windowHeight = windowHeight;

    bool failure = this->CreateContext();
}

Gui::~Gui() {
    glfwDestroyWindow(this->m_window);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Gui::ProcessInput(Camera* camera, Model* model) {
    if (glfwGetKey(this->m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(this->m_window, true);

    // move camera
    if (glfwGetKey(this->m_window, GLFW_KEY_A) == GLFW_PRESS)
        camera->Move(glm::vec3(-1.0, 0.0, 0.0));
    if (glfwGetKey(this->m_window, GLFW_KEY_D) == GLFW_PRESS)
        camera->Move(glm::vec3(1.0, 0.0, 0.0));
    if (glfwGetKey(this->m_window, GLFW_KEY_W) == GLFW_PRESS)
        camera->Move(glm::vec3(0.0, 1.0, 0.0));
    if (glfwGetKey(this->m_window, GLFW_KEY_S) == GLFW_PRESS)
        camera->Move(glm::vec3(0.0, -1.0, 0.0));
    if (glfwGetKey(this->m_window, GLFW_KEY_Q) == GLFW_PRESS)
        camera->Move(glm::vec3(0.0, 0.0, 1.0));
    if (glfwGetKey(this->m_window, GLFW_KEY_E) == GLFW_PRESS)
        camera->Move(glm::vec3(0.0, 0.0, -1.0));

    // rotate object
    if (glfwGetKey(this->m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
        model->Rotate(glm::vec3(0.0, 1.0, 0.0));
    if (glfwGetKey(this->m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        model->Rotate(glm::vec3(0.0, -1.0, 0.0));
    if (glfwGetKey(this->m_window, GLFW_KEY_UP) == GLFW_PRESS)
        model->Rotate(glm::vec3(1.0, 0.0, 0.0));
    if (glfwGetKey(this->m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
        model->Rotate(glm::vec3(-1.0, 0.0, 0.0));
}

bool Gui::CreateContext() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    this->m_window = glfwCreateWindow(static_cast<int>(this->m_windowWidth), static_cast<int>(this->m_windowHeight), "Global Ambient Occlusion", nullptr, nullptr);
    if (m_window == nullptr) {
        std::cout << "ERROR::GLFW - failed to create m_window\n" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(this->m_window);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
    ImGui_ImplOpenGL3_Init("#version 460");
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // setup parameters here to hot reload shade

    // setup GLAD
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cout << "ERROR::GLAD - failed to initialize GLAD\n" << std::endl;
        return EXIT_FAILURE;
    }

    return true;
}

void Gui::DrawImGui(float diffTime, int& nLights, bool& debug, ImGuiIO& io) {
    ImGui::Begin("Ambient Occlusion Settings");
    ImGui::Text("Calculating Global Ambient Occlusion took: %.3f seconds", diffTime);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::InputInt("# samples", &nLights, 1, 50);
    ImGui::Checkbox("Debug Mode", &debug);
    ImGui::End();
}

bool Gui::Run() {
    // setup viewport
    glViewport(0, 0, static_cast<int>(this->m_windowWidth), static_cast<int>(this->m_windowHeight));
    glfwSetFramebufferSizeCallback(
        this->m_window, [](GLFWwindow* win, const int width, const int height) { glViewport(0, 0, width, height); });

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // setup
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // AMBIENT OCCLUSION
    glm::mat4 projectionMatrix = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -10.0f, 10.0f);
    Model model("../../global-ao/resources/backpack.obj");
    OcclusionMap occlusionMap;
    int nLights = 1;
    GAOGenerator::ComputeOcclusion(model, nLights, occlusionMap);

    // texture rendering
    ShaderProgram textureShader("../../global-ao/shader/texture.vert", "../../global-ao/shader/texture.frag");
    Model quad("../../global-ao/resources/quad.obj");

    // occluded object rendering
    ShaderProgram gaoShader("../../global-ao/shader/gao.vert", "../../global-ao/shader/gao.frag");
    Camera camera(glm::vec3(0.0, 0.0, 1.0));

    // GUI stuff
    ImGuiIO& io = ImGui::GetIO();
    float diffTime = 0.;
    int priorSampleValue = 0;
    bool debug = false;

    // render loop
    while (!glfwWindowShouldClose(this->m_window)) {
        this->ProcessInput(&camera, &model);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // render texture (for debugging)
        if (debug) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            textureShader.Use();
            textureShader.SetInt("colorTexture", 0);

            glActiveTexture(GL_TEXTURE0);
            occlusionMap.BindTexture();
            quad.Draw();
        }

            // render model with occlusion texture
        else {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            gaoShader.Use();
            gaoShader.SetMat4("modelMatrix", model.GetModelMatrix());
            gaoShader.SetMat4("viewMatrix", camera.GetViewMatrix());
            gaoShader.SetMat4("projectionMatrix", projectionMatrix);
            gaoShader.SetInt("occlusionMap", 0);
            glActiveTexture(GL_TEXTURE0);
            occlusionMap.BindTexture();
            model.Draw();
        }

        // imgui
        this->DrawImGui(diffTime, nLights, debug, io);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // - imgui
        // store prior sample value to see if we need to compute GAO again
        if (priorSampleValue != nLights) {
            clock_t begin_time = clock();
            GAOGenerator::ComputeOcclusion(model, nLights, occlusionMap);
            diffTime = static_cast<float>(clock() - begin_time) / CLOCKS_PER_SEC;
            priorSampleValue = nLights;
        }

        glfwSwapBuffers(this->m_window);
        glfwPollEvents();
    }
    return EXIT_SUCCESS;
}
