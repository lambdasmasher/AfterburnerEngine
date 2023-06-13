#version 430 core

layout (vertices = 4) out;

uniform vec3 cameraPos;

float tessFactor(vec3 p1, vec3 p2) {
    float d1 = length(p1 - cameraPos);
    float d2 = length(p2 - cameraPos);
    float d = (d1 + d2) * 0.5;

    float minTess = 2.0;
    float maxTess = 32.0;
    float minDist = 10.0;
    float maxDist = 500.0;

    d = clamp((d - minDist) / (maxDist - minDist), 0.0, 1.0);

    return mix(maxTess, minTess, d);
}

void main(void) {
    if (gl_InvocationID == 0) {
        gl_TessLevelOuter[0] = tessFactor(gl_in[3].gl_Position.xyz, gl_in[0].gl_Position.xyz);
        gl_TessLevelOuter[1] = tessFactor(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz);
        gl_TessLevelOuter[2] = tessFactor(gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz);
        gl_TessLevelOuter[3] = tessFactor(gl_in[2].gl_Position.xyz, gl_in[3].gl_Position.xyz);
        gl_TessLevelInner[0] = mix(gl_TessLevelOuter[0], gl_TessLevelOuter[3], 0.5);
        gl_TessLevelInner[1] = mix(gl_TessLevelOuter[2], gl_TessLevelOuter[1], 0.5);
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
