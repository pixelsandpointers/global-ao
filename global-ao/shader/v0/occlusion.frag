#version 460 core

uniform sampler2D depthMap;
uniform sampler2D positionTex;
uniform sampler2D normalTex;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform vec3 viewDir;
uniform vec2 viewportSize;

out vec4 color;

void main(void)
{	
    vec2 fragTexcoord3D = vec2(gl_FragCoord.x / viewportSize.x, gl_FragCoord.y / viewportSize.y);

    vec4 normal = texture(normalTex, fragTexcoord3D);
    vec4 position = texture(positionTex, fragTexcoord3D);
    position = projectionMatrix * viewMatrix * modelMatrix * position;
    vec3 ndc = position.xyz / position.w;
    ndc = ndc * 0.5 + 0.5;
    
    float closestDepth = texture(depthMap, ndc.xy).r;
    float currentDepth = ndc.z;
    float shadow = currentDepth - 0.001 > closestDepth ? 1.0 : 0.0;
    
    if (shadow < 0.5) {
        shadow = max(dot(normalize(normal.xyz), normalize(viewDir)), 0.0);
    }
    
    color = vec4(vec3(shadow), 1.0);
}