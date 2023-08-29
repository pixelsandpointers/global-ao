#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec4 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 fragPosition;
out vec4 fragNormal;

void main()
{
	gl_Position = vec4(texcoord * 2.0 - 1.0, 0.0, 1.0);

    fragPosition = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
	fragNormal = vec4(normal, 1.0);
}