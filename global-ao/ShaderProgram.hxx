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

    /// Sets shader as currently active
    void Use() const;

    /// Sets no shader as currently active
    void Unuse() const;

    /// pass an uniform int value to shader
    /// \param uniformName used to denote value in the shader
    /// \param value to overwrite with the current value
    void SetInt(const char* uniformName, const int value) const;

    /// pass an uniform float value to shader
    /// \param uniformName used to denote value in the shader
    /// \param value to overwrite with the current value
    void SetFloat(const char* uniformName, const float value) const;

    /// pass an uniform vec2 value to shader
    /// \param uniformName used to denote value in the shader
    /// \param value to overwrite with the current value
    void SetVec2(const char* uniformName, const glm::vec2 value) const;

    /// pass an uniform vec3 value to shader
    /// \param uniformName used to denote value in the shader
    /// \param value to overwrite with the current value
    void SetVec3(const char* uniformName, const glm::vec3 value) const;

    /// pass an uniform mat4 value to shader
    /// \param uniformName used to denote value in the shader
    /// \param value to overwrite with the current value
    void SetMat4(const char* uniformName, const glm::mat4 value) const;
};
