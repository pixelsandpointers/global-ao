#version 460 core

in vec4 fragPositionLightSpace;

uniform sampler2D depthMap;
uniform float nSamples;

out vec4 color;

void main()
{            
    vec3 ndc = fragPositionLightSpace.xyz / fragPositionLightSpace.w;
    ndc = ndc * 0.5 + 0.5;
    float closestDepth = texture(depthMap, ndc.xy).r;
    float currentDepth = ndc.z;
    float shadow = currentDepth - 0.0001 > closestDepth ? 1.0 / nSamples : 0.0;

    color = vec4(vec3(shadow), 1.0);
}