#version 460 core

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexcoord;
in vec4 fragColor;

out vec4 color;

void main()
{
	vec3 lightPosition = vec3(1000.0, 1000.0, 1000.0);
	vec3 lightDir = normalize(lightPosition - fragPosition);
	float ambient = 0.1;
	float diffuse = max(dot(fragNormal, lightDir), 0.0);

	color = vec4((ambient + diffuse) * fragColor.rgb, 1.0);
}