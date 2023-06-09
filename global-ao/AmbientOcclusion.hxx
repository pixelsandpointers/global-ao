//
// Created by b on 6/9/23.
//

#ifndef GLOBAL_AO_AMBIENTOCCLUSION_HXX
#define GLOBAL_AO_AMBIENTOCCLUSION_HXX

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <random>
#include <vector>

class AmbientOcclusion {
  private:
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

  private:
    void setupBuffers(uint8_t screenWidth, uint8_t screenHeight);

  public:
    static float lerp(float a, float b, float f);
    float generateSample();
    std::vector<glm::vec3> generateSampleKernel(size_t numberOfSamples);
    std::vector<glm::vec3> generateNoiseTexture(size_t numberOfSamples);
};


#endif  //GLOBAL_AO_AMBIENTOCCLUSION_HXX
