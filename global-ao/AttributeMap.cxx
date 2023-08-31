#include "AttributeMap.hxx"

AttributeMap::AttributeMap(const unsigned int width, const unsigned int height, void* data) : m_WIDTH(width), m_HEIGHT(height) {
    // generate attribute map texture
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_WIDTH, m_HEIGHT, 0, GL_RGBA, GL_FLOAT, data);

    glBindTexture(GL_TEXTURE_2D, 0);
}

AttributeMap::~AttributeMap() {
    glDeleteTextures(1, &m_texture);
}

void AttributeMap::BindTexture() const {
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void AttributeMap::UnbindTexture() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}
