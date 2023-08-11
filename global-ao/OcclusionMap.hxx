#pragma once

#include <glad/gl.h>

class OcclusionMap {
  private:
    const unsigned int m_WIDTH = 1024, m_HEIGHT = 1024;
    unsigned int m_FBO, m_texture;
    int m_prevViewport[4] = { 0 };

  public:
    OcclusionMap(const unsigned int width = 1024, const unsigned int height = 1024)
      : m_WIDTH { width }, m_HEIGHT { height } {
        // generate framebuffer
        glGenFramebuffers(1, &m_FBO);

        // generate depth map texture
        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, m_WIDTH, m_HEIGHT, 0, GL_RED, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // attach texture to framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    ~OcclusionMap() {
        glDeleteTextures(1, &m_texture);
        glDeleteFramebuffers(1, &m_FBO);
    }

    void BindFramebuffer() {
        glGetIntegerv(GL_VIEWPORT, m_prevViewport);
        glViewport(0, 0, m_WIDTH, m_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void UnbindFramebuffer() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(m_prevViewport[0], m_prevViewport[1], m_prevViewport[2], m_prevViewport[3]);
    }

    void BindTexture() const {
        glBindTexture(GL_TEXTURE_2D, m_texture);
    }
};
