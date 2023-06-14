#version 430 core

layout (location = 0) in vec2 uv;

layout (location = 0) out vec4 colour;

layout (binding = 0) uniform sampler2D tex;
uniform float tiling;

void main(void) {
    colour = texture(tex, uv * tiling);
}
