//
// Created by b on 6/9/23.
//

#ifndef GLOBAL_AO_AMBIENTOCCLUSION_HXX
#define GLOBAL_AO_AMBIENTOCCLUSION_HXX

#include "Camera.hxx"
#include "Model.hxx"
#include "Shader.hxx"
//#include "Primitives.hxx"

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <vector>

using AttenuationParameters = std::pair<const float, const float>;

class AmbientOcclusion {
  private:
    // shader map
    std::map<ShadingPass, std::shared_ptr<Shader>> shaderMap;
    uint8_t screenWidth, screenHeight;
    // g-buffer vars
    GLuint gBuffer, gPosition, gNormal, gAlbedo;
    // depthbuffer
    GLuint rboDepth;
    // framebuffer
    GLuint aoFBO, aoBlurFBO;
    GLuint aoColorBuffer, aoColorBufferBlur;

    // sampler
    std::uniform_real_distribution<> randomFloats = std::uniform_real_distribution(0.0, 1.0);
    std::default_random_engine generator;

  public:
    AmbientOcclusion(uint8_t screenWidth, uint8_t screenHeight);
    ~AmbientOcclusion() = default;

    // shader management
    void setShader(const ShadingPass key, const char* vertexShaderName, const char* fragmentShaderName);
    Shader* getShaderPtr(const ShadingPass key) const;

    // computation
    static float lerp(float a, float b, float f);
    float generateSample();
    std::vector<glm::vec3> generateSampleKernel(uint8_t numberOfSamples);
    std::vector<glm::vec3> generateNoiseTexture(uint8_t numberOfSamples);

    // render passes
    void geometryPass(const Camera& camera, Model& object, const glm::mat4& projectionMat) const;
    void texturePass(uint8_t nSamplesKernel, uint8_t nSamplesNoise, const glm::mat4& projectionMat);
    void blurPass() const;
    void lightingPass(const Camera& camera, float (&lightPosition)[], float (&lightColor)[], AttenuationParameters attenuation) const;

    void setupFrameBuffers();
};


#endif  //GLOBAL_AO_AMBIENTOCCLUSION_HXX
