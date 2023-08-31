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

void Gui::ProcessInput(Scene& scene, float deltaTime) {
    if (glfwGetKey(this->m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(this->m_window, true);

    // turn camera
    if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
        scene.ProcessKeyboard(TURN_LEFT, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        scene.ProcessKeyboard(TURN_RIGHT, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
        scene.ProcessKeyboard(TURN_UP, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
        scene.ProcessKeyboard(TURN_DOWN, deltaTime);
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

void Gui::DrawImGui(float diffTime, int& nLights, bool& debug, bool& mode, bool& start, ImGuiIO& io) {
    ImGui::Begin("Ambient Occlusion Settings");
    ImGui::Text("Calculating Global Ambient Occlusion took: %.3f ms", diffTime);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::InputInt("# samples", &nLights, 1, 50);
    ImGui::Checkbox("AO Mode", &mode);
    ImGui::Checkbox("Debug Mode", &debug);
    if (ImGui::Button("Compute"))
        start = true;
    ImGui::End();
}

bool Gui::Run() {
    // setup viewport
    glViewport(0, 0, static_cast<int>(this->m_windowWidth), static_cast<int>(this->m_windowHeight));
    glfwSetFramebufferSizeCallback(
        this->m_window, [](GLFWwindow* win, const int width, const int height) { glViewport(0, 0, width, height); });

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // SCENE SETUP
    std::vector<Model> models;
    Model model("../../global-ao/resources/backpack.obj");
    model.Move(glm::vec3(0.0f, 0.0f, 0.0f));
    models.push_back(model);

    Model model2("../../global-ao/resources/backpack.obj");
    model2.Scale(glm::vec3(1.0f));
    model2.Move(glm::vec3(0.5f, 0.0f, 0.0f));
    model2.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 90);
    models.push_back(model2);

    Scene scene(models);

    int numLights = 1024;
    std::vector<OcclusionMap> occlusionMaps;
    // texture rendering
    ShaderProgram textureShader("../../global-ao/shader/texture.vert", "../../global-ao/shader/texture.frag");
    Model quad("../../global-ao/resources/quad.obj");

    // occluded object rendering
    ShaderProgram gaoShader("../../global-ao/shader/gao.vert", "../../global-ao/shader/gao.frag");
    gaoShader.Use();
    gaoShader.SetInt("occlusionMap", 0);
    gaoShader.Unuse();

    // GUI stuff and render parameters
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    ImGuiIO& io = ImGui::GetIO();
    float currentTime = 0.0f, lastTime = 0.0f, deltaTime = 0.0f;
    float diffTime = 0.0f;
    bool debug = false;
    bool mode = true, prevMode = true;
    bool start = false;

    // render loop
    while (!glfwWindowShouldClose(this->m_window)) {
        currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        this->ProcessInput(scene, deltaTime);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        // render texture (for debugging)
        if (debug) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            textureShader.Use();
            textureShader.SetInt("colorTexture", 0);

            glActiveTexture(GL_TEXTURE0);
            occlusionMaps[0].BindTexture();
            quad.Draw();
        }

        // render model with occlusion texture
        else {
            gaoShader.Use();
            gaoShader.SetInt("mode", mode);
            if (mode == 0) {
                int i = 0;
                for (Model& model : scene.GetModels()) {
                    gaoShader.SetMat4("modelMatrix", model.GetModelMatrix());
                    gaoShader.SetMat4("viewMatrix", scene.cam.GetViewMatrix());
                    gaoShader.SetMat4("projectionMatrix", scene.GetProjectionMatrix());
                    glActiveTexture(GL_TEXTURE0);
                    occlusionMaps[i].BindTexture();
                    model.Draw();
                    i++;
                }
            } else {
                scene.Render(gaoShader);
            }
        }
        // imgui
        this->DrawImGui(diffTime, numLights, debug, mode, start, io);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        if (start || mode != prevMode) {
            auto startTime = std::chrono::steady_clock::now();
            if (mode == 0) {
                occlusionMaps.clear();
                GAOGenerator::computeOcclusion1(scene, numLights, occlusionMaps);
            } else {
                GAOGenerator::computeOcclusion2(scene, numLights);
            }
            auto endTime = std::chrono::steady_clock::now();
            diffTime = std::chrono::duration<float, std::milli>(endTime - startTime).count();
            prevMode = mode;
            start = false;
        }

        glfwSwapBuffers(this->m_window);
        glfwPollEvents();
    }
    return EXIT_SUCCESS;
}
