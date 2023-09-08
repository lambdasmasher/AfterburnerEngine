#version 430 core

layout (location = 0) out vec4 outColour;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec4 outMaterial;

layout (location = 0) in vec2 uv;
layout (location = 1) in vec3 normal;

layout (binding = 0) uniform sampler2D modelTexture;

void main(void) {
    outColour = texture(modelTexture, uv);
    outColour.g += 0.05;
    if (outColour.a < 0.2) discard;
    outNormal = vec4(normal, 1.0);
    outMaterial = vec4(0.0, 1.0, 0.0, 1.7);
}
