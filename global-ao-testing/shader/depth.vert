#version 460 core

layout (location = 0) in vec3 position;

uniform mat4 modelMatrix;
uniform mat4 lightViewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * lightViewMatrix * modelMatrix * vec4(position, 1.0);
}