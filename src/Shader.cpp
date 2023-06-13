#include "Shader.hpp"

#include <fstream>
#include <sstream>

Shader::Shader(const char *vert,
                const char *tesc,
                const char *tese,
                const char *geom,
                const char *frag,
                const char *comp,
                const std::vector<std::string> &uniforms)
                : isCompute(comp != nullptr) {
    programId = glCreateProgram();
    attachShader(vert, GL_VERTEX_SHADER);
    attachShader(tesc, GL_TESS_CONTROL_SHADER);
    attachShader(tese, GL_TESS_EVALUATION_SHADER);
    attachShader(geom, GL_GEOMETRY_SHADER);
    attachShader(frag, GL_FRAGMENT_SHADER);
    attachShader(comp, GL_COMPUTE_SHADER);

    glLinkProgram(programId);
    int success;
    char log[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programId, 512, NULL, log);
        printf("failed to link shader program:\n%s\n", log);
        exit(0);
    }
    for (GLuint shader : shaders) {
        glDeleteShader(shader);
    }

    start();
    for (const std::string &s : uniforms) {
        locations[s] = glGetUniformLocation(programId, s.c_str());
    }
    stop();
}

Shader::~Shader() {
    glDeleteProgram(programId);
}

void Shader::attachShader(const char *path, GLenum type) {
    if (!path) return;
    std::string code;
    try {
        std::ifstream fin(path);
        fin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        std::stringstream buf;
        buf << fin.rdbuf();
        fin.close();
        code = buf.str();
    } catch (std::exception e) {
        printf("failed to read shader file %s\n", path);
        exit(0);
    }

    GLuint shader = glCreateShader(type);
    const char *source = code.c_str();
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    int success;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, log);
        printf("failed to compile shader file %s:\n%s\n", path, log);
        exit(0);
    }

    glAttachShader(programId, shader);
    shaders.push_back(shader);
}
