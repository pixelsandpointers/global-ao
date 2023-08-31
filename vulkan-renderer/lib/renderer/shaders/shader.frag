#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    float occlusion = texture(texSampler, fragTexCoord).r;
    float brightness = 1.0 - occlusion;
    outColor = vec4(vec3(brightness), 1.0);
}
