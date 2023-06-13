#version 430 core

layout (vertices = 4) out;

void main(void) {
    if (gl_InvocationID == 0) {
        for (int i = 0; i < 4; i++) {
            gl_TessLevelOuter[i] = 8.0;
        }
        gl_TessLevelInner[0] = mix(gl_TessLevelOuter[0], gl_TessLevelOuter[3], 0.5);
        gl_TessLevelInner[1] = mix(gl_TessLevelOuter[2], gl_TessLevelOuter[1], 0.5);
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
