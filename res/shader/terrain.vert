#version 430 core

uniform vec3 position;
uniform int numTiles;
uniform float tileSize;

vec3 vertices[4] = vec3[4](
    vec3(0.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0),
    vec3(1.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0)
);

void main(void) {
    int i = gl_InstanceID % numTiles;
    int j = gl_InstanceID / numTiles;
    gl_Position = vec4(position + tileSize * (vertices[gl_VertexID] + vec3(i, 0.0, j)), 1.0);
}
