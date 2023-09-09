#version 430 core

layout (location = 0) in vec2 uv;

layout (binding = 0) uniform sampler2D colourTexture;
layout (binding = 1) uniform sampler2D normalTexture;
layout (binding = 2) uniform sampler2D materialTexture;
layout (binding = 3) uniform sampler2D depthTexture;
layout (binding = 4) uniform samplerCube atmosphereTexture;
layout (binding = 5) uniform sampler2D refractionTexture;
layout (binding = 6) uniform sampler2DArray shadowMap;

layout (location = 0) out vec4 outColour;

uniform vec3 toLightVector;
uniform vec3 cameraPos;
uniform mat4 invProjMatrix;
uniform mat4 invViewMatrix;
uniform int numCascades;
uniform mat4 lightVps[8];
uniform float fogDensity;
uniform float fogGradient;

float ambientFactor = 0.3;
vec3 lightColour = vec3(1.0);
vec3 blinnPhong(vec3 colour, vec3 normal, vec3 position, vec4 materialData, vec3 skyColour) {
    vec3 toLightDir = normalize(toLightVector);
    vec3 ambient = colour * ambientFactor;
    vec3 diffuse = colour * max(dot(toLightDir, normal), 0.0);

    float reflectivity = materialData.r;
    float smoothness = materialData.g;

    vec3 viewDir = normalize(cameraPos - position);
    vec3 halfwayDir = normalize(toLightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), smoothness);
    vec3 specular = lightColour * spec * reflectivity;

    float shadow = 1.0;
    int layer;
    vec3 ndc;
    for (layer = 0; layer < numCascades; layer++) {
        vec4 clip = lightVps[layer] * vec4(position, 1.0);
        ndc = clip.xyz / clip.w;
        if (all(lessThan(abs(ndc), vec3(1.0)))) {
            break;
        }
    }
    ndc = ndc * 0.5 + 0.5;
    float cascades[] = {1, 100, 300, 1000, 2000, 5000};
    float bias = 4.0;
    bias *= (1.0 / (cascades[layer+1]));
    if (texture(shadowMap, vec3(ndc.xy, layer)).r + bias < ndc.z) {
        shadow = 0.5;
    }

    vec3 ret = ambient + shadow * (diffuse + specular);
    float dist = length(cameraPos - position);
    float visibility = exp(-pow((dist * fogDensity), fogGradient));
    visibility = clamp(visibility, 0.0, 1.0);
    return mix(skyColour, ret, visibility);
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
    vec4 skyColour = texture(atmosphereTexture, position - cameraPos);
    outColour = skyColour;
    if (rawNormal.a > 0.5) {
        outColour = vec4(blinnPhong(colour, normal, position, materialData, skyColour.rgb), 1.0);
        float gamma = materialData.a;
        if (gamma > 0.01) {
            outColour.rgb = pow(outColour.rgb, vec3(1.0 / gamma));
        }
        outColour = mix(outColour, texture(refractionTexture, uv), materialData.z);
    }
}
