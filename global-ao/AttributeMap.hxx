#pragma once

#include <glad/gl.h>

class AttributeMap {
  private:
    const unsigned int m_WIDTH, m_HEIGHT;
    unsigned int m_texture;

  public:
    AttributeMap(const unsigned int width, const unsigned int height, void* data);

    ~AttributeMap();

    void BindTexture() const;

    void UnbindTexture() const;
};
