#version 430 core

layout (location = 0) in vec2 uv;
layout (location = 1) in vec4 clipSpace;
layout (location = 2) in vec3 worldSpace;
layout (location = 3) in vec4 clipSpace0;

layout (location = 0) out vec4 colour;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 material;

layout (binding = 1) uniform sampler2D normalmap;
layout (binding = 2) uniform sampler2D reflectionMap;
layout (binding = 3) uniform sampler2D refractionMap;
layout (binding = 4) uniform sampler2D depthTexture;

uniform float tiling;
uniform vec3 cameraPos;
uniform float nearPlane;
uniform float farPlane;

float fresnel(vec3 normal) {
    vec3 eye = normalize(cameraPos - worldSpace);
    return clamp(pow(dot(eye, normal), 1.0), 0.0, 1.0);
}

float linearizeDepth(float d) {
    return 2.0 * nearPlane * farPlane / (farPlane + nearPlane - (2.0 * d - 1.0) * (farPlane - nearPlane));
}

vec2 clipSpaceToTexCoords(vec4 clip) {
    vec2 ndc = clip.xy / clip.w;
    return clamp(ndc / 2.0 + 0.5, 0.001, 0.999);
}

void main(void) {
    normal = vec4(normalize(texture(normalmap, uv * tiling).xyz), 1.0);
    material = vec4(1.0, 128.0, 0.0, 0.0);

    vec2 projCoord = clipSpaceToTexCoords(clipSpace);
    vec2 projCoord0 = clipSpaceToTexCoords(clipSpace0);
    vec2 reflectionCoord = vec2(projCoord0.x, 1.0 - projCoord0.y);
    vec2 refractionCoord = projCoord0;
    float fresnel = fresnel(normal.xyz);
    vec3 waterColour = vec3(0.1, 0.125, 0.19);

    vec3 reflection = texture(reflectionMap, reflectionCoord).xyz;
    reflection = mix(reflection*0.8, waterColour, 0.5);

    vec3 refraction = texture(refractionMap, refractionCoord).xyz;
    float depth = linearizeDepth(texture(depthTexture, projCoord).r) - linearizeDepth(gl_FragCoord.z);
    refraction = mix(refraction, waterColour, clamp(smoothstep(0.0, 10.0, depth), 0.5, 1.0));
    material.z = 1.0 - clamp(depth / 2.0, 0.0, 1.0);

    colour = vec4(mix(reflection, refraction, fresnel), 1.0);
}
