#version 460 core

in vec2 fragTexcoord;

uniform sampler2D currentMap;
uniform sampler2D newMap;

out vec4 color;

void main()
{            
    float value = texture(currentMap, fragTexcoord).r + texture(newMap, fragTexcoord).r;
    color = vec4(vec3(value), 1.0);
}