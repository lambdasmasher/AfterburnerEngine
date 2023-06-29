#version 430 core

layout (location = 0) in vec2 uv;

layout (location = 0) out vec4 colour;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 material;

layout (binding = 0) uniform sampler2D tex;
layout (binding = 2) uniform sampler2D normalmap;
layout (binding = 3) uniform sampler2D macroVariationTexture;

uniform float tiling;

vec3 microVariation() {
    vec2 texCoords = uv * tiling;
    float microTiling = 1.0;
    vec3 microTint = vec3(1.0, 1.0, 1.0);
    vec3 microColour = texture(tex, texCoords * microTiling).rgb * microTint;

    float microVariationTiling = 1.0;
    vec3 microVariationTint = vec3(0.75, 0.95, 0.51);
    vec3 microColourVariation = texture(tex, texCoords * microVariationTiling).g * microVariationTint;

    float microVariationAlphaTiling = 0.02;
    float microVariationBlend = texture(macroVariationTexture, texCoords * microVariationAlphaTiling).r;

    return mix(microColour, microColourVariation, 1.0 - microVariationBlend);
}

float macroVariation() {
    vec2 texCoords = uv * tiling * 0.5;
    float macroVariationNoise = (texture(macroVariationTexture, texCoords * 0.005).r + 0.5) *
                                (texture(macroVariationTexture, texCoords * 0.0015).r + 0.5);
    float noiseContrast = 0.7;
    return mix(noiseContrast, 1.0, macroVariationNoise);
}

void main(void) {
    colour = vec4(microVariation() * macroVariation(), 1.0);
    normal = vec4(texture(normalmap, uv).xyz, 1.0);
    material = vec4(0.0, 1.0, 0.0, 0.0);
}
