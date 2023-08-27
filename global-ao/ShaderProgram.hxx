#pragma once

#include <fstream>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <string>

class ShaderProgram {
  private:
    unsigned int m_ID;

  public:
    ShaderProgram(const char* vertPath, const char* fragPath);

    ~ShaderProgram();

    void Use() const;

    void SetInt(const char* uniformName, const int value) const;

    void SetFloat(const char* uniformName, const float value) const;

    void SetMat4(const char* uniformName, const glm::mat4 value) const;
};

