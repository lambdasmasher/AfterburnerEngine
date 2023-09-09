#version 430 core

layout (triangles, invocations = 5) in;
layout (triangle_strip, max_vertices = 3) out;

layout (location = 0) in vec2 uv[];
layout (location = 0) out vec2 outUv;

uniform mat4 vpMatrices[8];

void main(void) {
    for (int i = 0; i < 3; i++) {
        outUv = uv[i];
        gl_Position = vpMatrices[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}
