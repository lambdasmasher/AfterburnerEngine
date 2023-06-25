#version 430 core

layout (location = 0) in vec2 uv;

layout (location = 0) out vec4 colour;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 material;

layout (binding = 1) uniform sampler2D normalmap;
uniform float tiling;

void main(void) {
    colour = vec4(0.164, 0.185, 0.22, 1.0);
    normal = vec4(texture(normalmap, uv * tiling).xyz, 1.0);
    material = vec4(1.0, 128.0, 0.0, 0.0);
}
