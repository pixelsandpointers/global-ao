#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 texcoord;

layout (location = 0) out vec2 fragTexcoord;

void main()
{
	gl_Position = vec4(texcoord * 2.0 - 1.0, 0.0, 1.0);

	fragTexcoord = texcoord;
}