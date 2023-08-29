#version 460 core

in vec2 fragTexcoord;
in vec4 fragColor;

uniform sampler2D occlusionMap;

uniform int mode;

out vec4 color;

void main()
{
	if (mode == 0) {
		color = vec4(vec3(1.0 - fragColor), 1.0);
	}
	else {
		float occlusion = texture(occlusionMap, fragTexcoord).r;
		float brightness = 1.0 - occlusion;
		color = vec4(vec3(brightness), 1.0);		
	}
}