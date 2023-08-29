#pragma once

#include <glad/gl.h>

class OcclusionMap {
  public:
    const unsigned int m_WIDTH, m_HEIGHT;
    unsigned int m_FBO, m_texture;
    int m_prevViewport[4] = { 0 };

  public:
    OcclusionMap(const unsigned int width = 1024, const unsigned int height = 1024);

    ~OcclusionMap();

    void BindFramebuffer();

    void UnbindFramebuffer() const;

    void BindTexture() const;

    void UnbindTexture() const;

    void ReadData(void* buffer) const;
};
