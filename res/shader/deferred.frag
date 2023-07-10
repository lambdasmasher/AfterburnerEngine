#version 430 core

layout (location = 0) in vec2 uv;

layout (binding = 0) uniform sampler2D colourTexture;
layout (binding = 1) uniform sampler2D normalTexture;
layout (binding = 2) uniform sampler2D materialTexture;
layout (binding = 3) uniform sampler2D depthTexture;
layout (binding = 4) uniform samplerCube atmosphereTexture;
layout (binding = 5) uniform sampler2D refractionTexture;

layout (location = 0) out vec4 outColour;

uniform vec3 toLightVector;
uniform vec3 cameraPos;
uniform mat4 invProjMatrix;
uniform mat4 invViewMatrix;

float ambientFactor = 0.3;
vec3 lightColour = vec3(1.0);
vec3 blinnPhong(vec3 colour, vec3 normal, vec3 position, vec4 materialData) {
    vec3 toLightDir = normalize(toLightVector);
    vec3 ambient = colour * ambientFactor;
    vec3 diffuse = colour * max(dot(toLightDir, normal), 0.0);

    float reflectivity = materialData.r;
    float smoothness = materialData.g;

    vec3 viewDir = normalize(cameraPos - position);
    vec3 halfwayDir = normalize(toLightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), smoothness);
    vec3 specular = lightColour * spec * reflectivity;

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
    vec4 rawNormal = texture(normalTexture, uv);
    vec3 normal = normalize(rawNormal.xyz);
    vec3 position = worldPosFromDepth(texture(depthTexture, uv).r);
    vec4 materialData = texture(materialTexture, uv);
    if (rawNormal.a > 0.5) {
        outColour = vec4(blinnPhong(colour, normal, position, materialData), 1.0);
        outColour = mix(outColour, texture(refractionTexture, uv), materialData.z);
    } else {
        outColour = texture(atmosphereTexture, position - cameraPos);
    }
}
