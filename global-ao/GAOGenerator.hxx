#pragma once

#include "Camera.hxx"
#include "DepthMap.hxx"
#include "Model.hxx"
#include "OcclusionMap.hxx"
#include "ShaderProgram.hxx"
#include "Scene.hxx"
#include "AttributeMap.hxx"

#include <glad/gl.h> 
#include <GLFW/glfw3.h>

using Light = Camera;

namespace GAOGenerator {
void computeOcclusion0(Scene& const scene, int numLights);

void computeOcclusion1(Scene& const scene, int numLights, std::vector<OcclusionMap>& occlusionMaps);
/*
void computeOcclusion2(
    const Scene& const scene,
    const Model& const model,
    int nLights,
    OcclusionMap& const accumMap2,
    GLFWwindow* window);

void computeOcclusion3(const Model& const model, int nLights, OcclusionMap& const accumMap2);

void computeOcclusion4(
    Scene& const scene,
    Model& model,
    int nLights,
    OcclusionMap& const accumMap2,
    GLFWwindow* window);
*/
};  // namespace GAOGenerator