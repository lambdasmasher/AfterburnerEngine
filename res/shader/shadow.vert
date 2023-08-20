#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 3) in mat4 modelMatrix;

layout (location = 0) out vec2 outUv;

void main(void) {
    outUv = uv;
    gl_Position = modelMatrix * vec4(position, 1.0);
}
