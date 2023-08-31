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

namespace GAOGenerator {
void computeOcclusion2(Scene& scene, int numLights);

void computeOcclusion1(Scene& scene, int numLights, std::vector<OcclusionMap>& occlusionMaps);

/*
void computeOcclusion2(
    const Scene& const scene,
    const Model& const model,
    int nLights,
    OcclusionMap& const accumMap2,
    GLFWwindow* window);
*/

};  // namespace GAOGenerator