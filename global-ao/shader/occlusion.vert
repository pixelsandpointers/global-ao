#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 fragPosition;

void main()
{
	gl_Position = vec4(texcoord * 2.0 - 1.0, 0.0, 1.0);

    fragPosition = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
}