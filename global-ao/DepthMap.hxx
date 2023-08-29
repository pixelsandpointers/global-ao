#pragma once

#include <glad/gl.h>

class DepthMap {
  private:
    const unsigned int m_WIDTH, m_HEIGHT;
    unsigned int m_FBO, m_texture;
    int m_prevViewport[4] = { 0 };

  public:
    /// Generates a depth map texture and attaches the texture to the framebuffer
    /// \param width of the texture
    /// \param height of the texture
    DepthMap(const unsigned int width = 1024, const unsigned int height = 1024);

    ~DepthMap();

    void BindFramebuffer();

    void UnbindFramebuffer() const;

    void BindTexture() const;
};