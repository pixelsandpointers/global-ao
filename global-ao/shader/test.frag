#version 460 core

in vec3 fragPosition;
in vec3 fragNormal;

out vec4 color;

void main()
{
	vec3 lightPosition = vec3(1.0, 1.0, 1.0);
	vec3 lightDir = normalize(lightPosition - fragPosition);
	float ambient = 0.1;
	float diffuse = max(dot(fragNormal, lightDir), 0.0);

	color = vec4(ambient + diffuse * vec3(1.0), 1.0);
}