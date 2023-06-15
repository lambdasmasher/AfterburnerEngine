#version 430 core

vec2 vertices[6] = vec2[6](
    vec2(1, 1), vec2(-1, 1), vec2(-1, -1),
    vec2(-1, -1), vec2(1, -1), vec2(1, 1)
);

layout (location = 0) out vec2 uv;

void main(void) {
    gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
    uv = vertices[gl_VertexID] * 0.5 + 0.5;
}
