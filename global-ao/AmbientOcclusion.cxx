//
// Created by b on 6/9/23.
//

#include "AmbientOcclusion.hxx"

std::vector<glm::vec3> AmbientOcclusion::generateSampleKernel(size_t numberOfSamples) {
    std::vector<glm::vec3> kernel;

    for (size_t i = 0; i < numberOfSamples; ++i)
    {
        glm::vec3 sample(generateSample() * 2. - 1., generateSample() * 2. - 1., generateSample());
        sample = glm::normalize(sample);
        sample *= generateSample();
        float scale = static_cast<float>(i) / static_cast<float>(numberOfSamples);
        scale = lerp(.1f, 1.f, scale * scale);
        sample *= scale;
        kernel.push_back(sample);
    }

    return kernel;
}

float AmbientOcclusion::lerp(float a, float b, float f) {
    return a + f * (b-a);
}

std::vector<glm::vec3> AmbientOcclusion::generateNoiseTexture(size_t numberOfSamples) {
    std::vector<glm::vec3> noise;
    for (size_t i = 0; i < numberOfSamples; ++i) {
        noise.push_back(glm::vec3(generateSample() * 2.0 - 1.0, generateSample() * 2.0 - 1.0, .0f));
    }

    return noise;
}

float AmbientOcclusion::generateSample() {
    return static_cast<float>(this->randomFloats(this->generator));
}

void AmbientOcclusion::setupBuffers(uint8_t screenWidth, uint8_t screenHeight) {
    glGenFramebuffers(1, &this->gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, this->gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &this->gNormal);
    glBindTexture(GL_TEXTURE_2D, this->gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &this->gAlbedo);
    glBindTexture(GL_TEXTURE_2D, this->gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, this->gAlbedo, 0);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &this->rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, this->rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &this->aoFBO);  glGenFramebuffers(1, &this->aoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, this->aoFBO);
    // AO color buffer
    glGenTextures(1, &this->aoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, this->aoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->aoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    // and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, this->aoBlurFBO);
    glGenTextures(1, &this->aoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, this->aoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->aoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
 }
