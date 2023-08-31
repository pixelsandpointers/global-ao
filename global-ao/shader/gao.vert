#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec4 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 fragTexcoord;
out vec4 fragColor;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
	fragTexcoord = texcoord;
	fragColor = color;
}