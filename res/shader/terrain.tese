#version 430 core

layout(quads, fractional_even_spacing, ccw) in;

uniform mat4 vpMatrix;

void main(void) {
    vec4 pos1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
    vec4 pos2 = mix(gl_in[3].gl_Position, gl_in[2].gl_Position, gl_TessCoord.x);
    vec4 pos = mix(pos1, pos2, gl_TessCoord.y);
    gl_Position = vpMatrix * pos;
}
