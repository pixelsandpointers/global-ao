#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 texcoord;

layout(binding = 0) uniform UniformBufferObject {
    mat4 modelMatrix;
    mat4 lightViewMatrix;
    mat4 projectionMatrix;
} ubo;

layout (location = 0) out vec4 fragPositionLightSpace;

void main()
{
    gl_Position = vec4(texcoord * 2.0 - 1.0, 0.0, 1.0);

    vec4 positionLightSpace = ubo.projectionMatrix * ubo.lightViewMatrix * ubo.modelMatrix * vec4(position, 1.0);
    fragPositionLightSpace = positionLightSpace;
}