//
// Created by b on 20.08.23.
//

#ifndef GLOBAL_AO_GUI_HXX
#define GLOBAL_AO_GUI_HXX

#include "Camera.hxx"
#include "Model.hxx"
#include "OcclusionMap.hxx"
#include "ShaderProgram.hxx"
#include "GAOGenerator.hxx"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <stb_image.h>

class Gui {
  public:
    Gui(unsigned int windowWidth, unsigned int windowHeight);
    ~Gui();
    bool Run();

  private:
    GLFWwindow* window = nullptr;
    unsigned int windowWidth, windowHeight;
    void ProcessInput(Camera* camera, Model* model);
    bool CreateContext();
    void DrawImGui(float diffTime, int& nLights, bool& debug, ImGuiIO& io);
};


#endif  //GLOBAL_AO_GUI_HXX
