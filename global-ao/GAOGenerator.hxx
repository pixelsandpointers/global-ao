#pragma once

#include "ShaderProgram.hxx"
#include "DepthMap.hxx"
#include "OcclusionMap.hxx"
#include "Camera.hxx"
#include "Model.hxx"

using Light = Camera;

namespace GAOGenerator {
    void ComputeOcclusion(Model& model, int nLights, OcclusionMap& accumMap2);
};