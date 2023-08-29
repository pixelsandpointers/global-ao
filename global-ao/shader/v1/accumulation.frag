#version 460 core

in vec2 fragTexcoord;

uniform sampler2D currentMap;
uniform sampler2D newMap1;
uniform sampler2D newMap2;
uniform sampler2D newMap3;
uniform sampler2D newMap4;
uniform sampler2D newMap5;
uniform sampler2D newMap6;
uniform sampler2D newMap7;
uniform sampler2D newMap8;

out vec4 color;

void main()
{    
    float value0 = texture(currentMap, fragTexcoord).r;        
    float value1 = texture(newMap1, fragTexcoord).r;
    float value2 = texture(newMap2, fragTexcoord).r;
    float value3 = texture(newMap3, fragTexcoord).r;
    float value4 = texture(newMap4, fragTexcoord).r;
    float value5 = texture(newMap5, fragTexcoord).r;
    float value6 = texture(newMap6, fragTexcoord).r;
    float value7 = texture(newMap7, fragTexcoord).r;
    float value8 = texture(newMap8, fragTexcoord).r;
    float value = value0 + value1 + value2 + value3 + value4 + value5 + value6 + value7 + value8;
    color = vec4(vec3(value), 1.0);
}