#version 430 core

layout (location = 0) in vec3 position;
layout (location = 3) in mat4 modelMatrix;

void main(void) {
    gl_Position = modelMatrix * vec4(position, 1.0);
}
