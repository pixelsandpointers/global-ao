#version 460 core

in vec2 fragTexcoord;

uniform sampler2D occlusionMap;

out vec4 color;

void main()
{
	float occlusion = texture(occlusionMap, fragTexcoord).r;
	float brightness = 1.0 - occlusion;
	color = vec4(vec3(brightness), 1.0);
}