#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

layout (location = 0) out vec2 outUv;
layout (location = 1) out vec3 outNormal;

uniform mat4 modelMatrix;
uniform mat4 vpMatrix;

void main(void) {
    gl_Position = vpMatrix * modelMatrix * vec4(position, 1.0);
    outUv = uv;
    outNormal = (modelMatrix * vec4(normal, 0.0)).xyz;
}
