#pragma once

#include "Camera.hxx"
#include "Model.hxx"
#include "OcclusionMap.hxx"
#include "ShaderProgram.hxx"
#include "GAOGenerator.hxx"
#include "Scene.hxx"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <stb_image.h>
#include <chrono>

class Gui {
  public:
    /// Generates the glfw GUI and sets the context to the m_window
    /// \param windowWidth glfw m_window width
    /// \param windowHeight glfw m_window height
    Gui(unsigned int windowWidth, unsigned int windowHeight);
    ~Gui();

    /// Runs the application/renderer
    /// \return boolean, whether the application fails or not
    bool Run();

  private:
    GLFWwindow* m_window = nullptr;
    unsigned int m_windowWidth, m_windowHeight;

    /// Processes the event loop within a glfw m_window
    /// \param camera the camera position to change
    /// \param model the model to manipulate
    void ProcessInput(Scene& scene, float deltaTime);

    /// Creates the context for GLFW and ImGui
    /// \return a bool depending on whether initialization was successful
    bool CreateContext();

    /// Draw the ImGui controls in the glfw context
    /// \param diffTime the difference time between the frames rendered
    /// \param nLights the amount of samples in the sphere around the model
    /// \param debug whether or not to render the texture view
    /// \param io the ImGui io controller
    void DrawImGui(float diffTime, int& nLights, bool& debug, bool& mode, bool& start, ImGuiIO& io);
};
