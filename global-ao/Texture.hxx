#pragma once

#include "glad/gl.h"

#include "stb_image.h"
#include "iostream"

class Texture {
  private:
      unsigned int m_ID;

  public:
    Texture(const char *path);

    ~Texture();;

    void bind();
};
