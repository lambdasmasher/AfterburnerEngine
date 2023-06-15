#version 430 core

layout (location = 0) in vec2 uv;

layout (binding = 0) uniform sampler2D colourTexture;
layout (binding = 1) uniform sampler2D normalTexture;
layout (binding = 2) uniform sampler2D depthTexture;

layout (location = 0) out vec4 outColour;

uniform vec3 cameraPos;
uniform mat4 invProjMatrix;
uniform mat4 invViewMatrix;

float ambientFactor = 0.3;
vec3 toLightDir = normalize(vec3(0.0, 1.0, 0.5));
vec3 lightColour = vec3(1.0);
vec3 blinnPhong(vec3 colour, vec3 normal, vec3 position) {
    vec3 ambient = colour * ambientFactor;
    vec3 diffuse = colour * max(dot(toLightDir, normal), 0.0);

    vec3 viewDir = normalize(cameraPos - position);
    vec3 halfwayDir = normalize(toLightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = lightColour * spec * 0.0;

    return ambient + diffuse + specular;
}

vec3 worldPosFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    vec4 clipSpace = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 viewSpace = invProjMatrix * clipSpace;
    viewSpace /= viewSpace.w;
    vec4 worldSpace = invViewMatrix * viewSpace;
    return worldSpace.xyz;
}

void main(void) {
    vec3 colour = texture(colourTexture, uv).rgb;
    vec3 normal = normalize(texture(normalTexture, uv).xyz);
    vec3 position = worldPosFromDepth(texture(depthTexture, uv).r);
    outColour = vec4(blinnPhong(colour, normal, position), 1.0);
}
