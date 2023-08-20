#version 430 core

layout (location = 0) in vec2 uv;

layout (binding = 0) uniform sampler2D modelTexture;

void main(void) {
    if (texture(modelTexture, uv).a < 0.5) {
        discard;
    }
}
