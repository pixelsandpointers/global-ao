#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

class ShaderProgram
{
private:
	unsigned int m_ID;

public:
	ShaderProgram(const char* vertPath, const char* fragPath)
	{
		std::string shaderString;
		std::ifstream file;
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		int success;
		char infoLog[1024];

		// setup vertex shader
		unsigned int vertID;
		vertID = glCreateShader(GL_VERTEX_SHADER);
		try
		{
			file.open(vertPath);
			std::stringstream buffer;
			buffer << file.rdbuf();
			file.close();
			shaderString = buffer.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER - failed to read vertex shader:\n" << e.what() << std::endl;
		}
		const char* vertShader = shaderString.c_str();
		glShaderSource(vertID, 1, &vertShader, NULL);
		glCompileShader(vertID);

		glGetShaderiv(vertID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertID, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER - failed to compile vertex shader:\n" << infoLog << std::endl;
		}

		// setup fragment shader
		unsigned int fragID;
		fragID = glCreateShader(GL_FRAGMENT_SHADER);
		try
		{
			file.open(fragPath);
			std::stringstream buffer;
			buffer << file.rdbuf();
			file.close();
			shaderString = buffer.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER - failed to read fragment shader:\n" << e.what() << std::endl;
		}
		const char* fragShader = shaderString.c_str();
		glShaderSource(fragID, 1, &fragShader, NULL);
		glCompileShader(fragID);

		glGetShaderiv(fragID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragID, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER - failed to compile fragment shader:\n" << infoLog << std::endl;
		}

		// setup program
		m_ID = glCreateProgram();
		glAttachShader(m_ID, vertID);
		glAttachShader(m_ID, fragID);
		glLinkProgram(m_ID);

		// delete shaders after attaching them to program
		glDeleteShader(vertID);
		glDeleteShader(fragID);
	}
	~ShaderProgram()
	{
		glDeleteProgram(m_ID);
	}

	void use() const
	{
		glUseProgram(m_ID);
	}

	void setMat4(const char* uniformName, const glm::mat4 value) const
	{
		int loc = glGetUniformLocation(m_ID, uniformName);
		if (loc == -1)
			std::cout << "ERROR::UNIFORM - failed to find uniform [" << uniformName << "]" << std::endl;
		glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
	}
};
