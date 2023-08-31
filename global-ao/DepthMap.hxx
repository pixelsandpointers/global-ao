#pragma once

#include <glad/gl.h>

class DepthMap {
  private:
    unsigned int m_FBO, m_texture;
    const unsigned int m_WIDTH, m_HEIGHT;
    int m_prevViewport[4] = { 0 };

  public:
    DepthMap(const unsigned int width = 1024, const unsigned int height = 1024);

    ~DepthMap();

    void BindFramebuffer();

    void UnbindFramebuffer() const;

    void BindTexture() const;
};