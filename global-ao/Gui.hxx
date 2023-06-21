//
// Created by b on 6/21/23.
//

#ifndef GLOBAL_AO_GUI_HXX
#define GLOBAL_AO_GUI_HXX

#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

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
    int run();
};

#endif  //GLOBAL_AO_GUI_HXX
