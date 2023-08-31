#include "DepthMap.hxx"

DepthMap::DepthMap(const unsigned int width, const unsigned int height) : m_WIDTH(width), m_HEIGHT(height) {
    // generate depth map texture
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_WIDTH, m_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    // generate framebuffer and attach texture
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

DepthMap::~DepthMap() {
    glDeleteTextures(1, &m_texture);
    glDeleteFramebuffers(1, &m_FBO);
}

void DepthMap::BindFramebuffer() {
    glGetIntegerv(GL_VIEWPORT, m_prevViewport);
    glViewport(0, 0, m_WIDTH, m_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void DepthMap::UnbindFramebuffer() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(m_prevViewport[0], m_prevViewport[1], m_prevViewport[2], m_prevViewport[3]);
}

void DepthMap::BindTexture() const {
    glBindTexture(GL_TEXTURE_2D, m_texture);
}
