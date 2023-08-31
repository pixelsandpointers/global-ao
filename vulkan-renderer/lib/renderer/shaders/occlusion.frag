#version 450

layout (location = 0) in vec4 fragPositionLightSpace;

layout(binding = 1) uniform sampler2D depthMap;
layout(binding = 2) uniform UniformBufferObject {
    float nSamples;
} ubo;

layout (location = 0) out vec4 color;

void main()
{            
    vec3 ndc = fragPositionLightSpace.xyz / fragPositionLightSpace.w;
    ndc = ndc * 0.5 + 0.5;
    float closestDepth = texture(depthMap, ndc.xy).r;
    float currentDepth = ndc.z;
    float shadow = currentDepth - 0.001 > closestDepth ? 1.0 / ubo.nSamples : 0.0;

    color = vec4(vec3(shadow), 1.0);
}