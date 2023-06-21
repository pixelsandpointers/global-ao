//
// Created by b on 6/9/23.
//

#include "AmbientOcclusion.hxx"


AmbientOcclusion::AmbientOcclusion(uint8_t screenWidth, uint8_t screenHeight): screenHeight(screenWidth), screenWidth(screenWidth) {
    // TODO:
    // setup framebuffers here
}

std::vector<glm::vec3> AmbientOcclusion::generateSampleKernel(uint8_t numberOfSamples) {
    std::vector<glm::vec3> kernel;

    for (uint8_t i = 0; i < numberOfSamples; ++i) {
        glm::vec3 sample(this->generateSample() * 2. - 1., this->generateSample() * 2. - 1., this->generateSample());
        sample = glm::normalize(sample);
        sample *= this->generateSample();
        float scale = static_cast<float>(i) / static_cast<float>(numberOfSamples);
        scale = lerp(.1f, 1.f, scale * scale);
        sample *= scale;
        kernel.push_back(sample);
    }

    return kernel;
}

float AmbientOcclusion::lerp(float a, float b, float f) {
    return a + f * (b - a);
}

std::vector<glm::vec3> AmbientOcclusion::generateNoiseTexture(uint8_t numberOfSamples) {
    std::vector<glm::vec3> noise;
    for (uint8_t i = 0; i < numberOfSamples; ++i) {
        noise.push_back(glm::vec3(this->generateSample() * 2.0 - 1.0, this->generateSample() * 2.0 - 1.0, .0f));
    }

    return noise;
}

float AmbientOcclusion::generateSample() {
    return static_cast<float>(this->randomFloats(this->generator));
}

void AmbientOcclusion::setupFrameBuffers() {
    glGenFramebuffers(1, &this->gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    // position color buffer
    glGenTextures(1, &this->gPosition);
    glBindTexture(GL_TEXTURE_2D, this->gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->screenWidth, this->screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->gPosition, 0);
    // normal color buffer
    glGenTextures(1, &this->gNormal);
    glBindTexture(GL_TEXTURE_2D, this->gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->screenWidth, this->screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &this->gAlbedo);
    glBindTexture(GL_TEXTURE_2D, this->gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->screenWidth, this->screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, this->gAlbedo, 0);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &this->rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, this->rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->screenWidth, this->screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &this->aoFBO);
    glGenFramebuffers(1, &this->aoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, this->aoFBO);
    // AO color buffer
    glGenTextures(1, &this->aoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, this->aoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->screenWidth, this->screenHeight, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->aoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    // and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, this->aoBlurFBO);
    glGenTextures(1, &this->aoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, this->aoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->screenWidth, this->screenHeight, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->aoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void AmbientOcclusion::setShader(const std::string& key, const char* vertexShaderName, const char* fragmentShaderName) {
    Shader shadingProgram(vertexShaderName, fragmentShaderName);
    this->shaderMap[key] = std::make_shared<Shader>(shadingProgram);
}

Shader* AmbientOcclusion::getShaderPtr(const std::string& key) const {
    if (!this->shaderMap.contains(key)) {
        return nullptr;
    }

    // return shaderProgramPointer
    return this->shaderMap.at(key).get();
}

void AmbientOcclusion::geometryPass(const Camera& camera, Model object, const glm::mat4& projectionMat) const {
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    // deref Shader program pointer in map
    Shader* shaderProgramPtr = this->getShaderPtr("geometry");
    shaderProgramPtr->use();
    shaderProgramPtr->setMat4("projection", projectionMat);
    shaderProgramPtr->setMat4("view", view);
    // room cube
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0, 7.0f, 0.0f));
    model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
    shaderProgramPtr->setMat4("model", model);
    shaderProgramPtr->setInt("invertedNormals", 1);  // invert normals as we're inside the cube
    //renderCube();
    shaderProgramPtr->setInt("invertedNormals", 0);
    // backpack model on the floor
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
    model = glm::scale(model, glm::vec3(1.0f));
    shaderProgramPtr->setMat4("model", model);
    //object.Draw(shaderProgramPtr);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// TODO: fix const problem when using `this->`
//void AmbientOcclusion::texturePass(uint8_t nSamples, const glm::mat4& projectionMat) const {
//    std::vector<glm::vec3> aoKernel = this->generateSampleKernel(nSamples);
//    glBindFramebuffer(GL_FRAMEBUFFER, aoFBO);
//    glClear(GL_COLOR_BUFFER_BIT);
//    Shader* shaderProgramPtr = this->getShaderPtr("texture");
//    shaderProgramPtr->use();
//    // Send kernel + rotation
//    for (unsigned int i = 0; i < 64; ++i)
//        shaderProgramPtr->setVec3("samples[" + std::to_string(i) + "]", aoKernel[i]);
//    shaderProgramPtr->setMat4("projection", projectionMat);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, gPosition);
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, gNormal);
//    glActiveTexture(GL_TEXTURE2);
//    glBindTexture(GL_TEXTURE_2D, this.
//    renderQuad();
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//}

