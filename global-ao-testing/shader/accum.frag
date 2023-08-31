#version 460 core

in vec4 fragPosition;

uniform sampler2D tex1;
uniform sampler2D tex2;

out vec4 color;

void main()
{
	vec3 fragTexcoord = (fragPosition.xyz / fragPosition.w) * 0.5 + 0.5;
	float value = texture(tex1, fragTexcoord.xy).r + texture(tex2, fragTexcoord.xy).r;
	color = vec4(vec3(value / 2.0), 1.0);
}