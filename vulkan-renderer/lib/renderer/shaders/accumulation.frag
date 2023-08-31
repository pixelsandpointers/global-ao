#version 450 core

layout(location = 0) in vec2 fragTexcoord;

layout(binding = 0) uniform sampler2D currentMap;
layout(binding = 1) uniform sampler2D newMap;

layout(location = 0) out vec4 color;

void main()
{            
    float value = texture(currentMap, fragTexcoord).r + texture(newMap, fragTexcoord).r;
    color = vec4(vec3(value), 1.0);
}