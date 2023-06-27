//
// Created by b on 6/21/23.
//

#ifndef GLOBAL_AO_GUI_HXX
#define GLOBAL_AO_GUI_HXX

#include <iostream>
#include <map>
#include <string>
#include <variant>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "AmbientOcclusion.hxx"
#include "Camera.hxx"
#include "Model.hxx"


class Gui {
  private:
    const int windowWidth, windowHeight;
    GLFWwindow* window;

  private:
    bool glfwCreateContext(int width, int height, const char* windowTitle);
    void setupImGui();
    void processInput(GLFWwindow* window);

  public:
    Gui(int width, int height, const char* title);
    ~Gui();
    int run(std::map<string, std::variant<std::string, bool, int, float>>& params);
};

#endif  //GLOBAL_AO_GUI_HXX
