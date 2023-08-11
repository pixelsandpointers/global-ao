#pragma once

#include "glad/gl.h"

#include "stb_image.h"
#include "iostream"

class Texture {
  private:
      unsigned int m_ID;

  public:
    Texture(const char *path)
    {
        glGenTextures(1, &m_ID);
        glBindTexture(GL_TEXTURE_2D, m_ID);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load and generate the texture
        int width, height, channels;
        unsigned char* data =
            stbi_load(path, &width, &height, &channels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "ERROR::TEXTURE - Failed to load texture [failure: '" << stbi_failure_reason() << "'] " << std::endl;
        }
        stbi_image_free(data);
    }

    ~Texture()
    {
        glDeleteTextures(1, &m_ID);
    };

    void bind()
    {
        glBindTexture(GL_TEXTURE_2D, m_ID);
    }
};
