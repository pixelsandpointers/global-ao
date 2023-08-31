#include "OcclusionMap.hxx"

OcclusionMap::OcclusionMap(const unsigned int width, const unsigned int height) : m_WIDTH(width), m_HEIGHT(height) {
    // generate depth map texture
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_WIDTH, m_HEIGHT, 0, GL_RGBA, GL_FLOAT, 0);

    // generate framebuffer and attach texture
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

OcclusionMap::~OcclusionMap() {
    glDeleteTextures(1, &m_texture);
    glDeleteFramebuffers(1, &m_FBO);
}

void OcclusionMap::BindFramebuffer() {
    glGetIntegerv(GL_VIEWPORT, m_prevViewport);
    glViewport(0, 0, m_WIDTH, m_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

void OcclusionMap::UnbindFramebuffer() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(m_prevViewport[0], m_prevViewport[1], m_prevViewport[2], m_prevViewport[3]);
}

void OcclusionMap::BindTexture() const {
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void OcclusionMap::UnbindTexture() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OcclusionMap::ReadData(void* buffer) const {
    glGetTextureSubImage(m_texture, 0, 0, 0, 0, m_WIDTH, m_HEIGHT, 1, GL_RGBA, GL_FLOAT, m_WIDTH * m_HEIGHT * 4 * sizeof(float), buffer);
}