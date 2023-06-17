#version 430 core

layout (location = 0) in vec2 uv;

layout (location = 0) out vec4 colour;
layout (location = 1) out vec4 normal;

layout (binding = 0) uniform sampler2D tex;
layout (binding = 2) uniform sampler2D normalmap;
uniform float tiling;

void main(void) {
    colour = texture(tex, uv * tiling);
    normal = vec4(texture(normalmap, uv).xyz, 1.0);
}
