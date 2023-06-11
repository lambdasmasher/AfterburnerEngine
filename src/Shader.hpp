#ifndef SHADER
#define SHADER

#include <GL/glew.h>
#include <unordered_map>
#include <vector>
#include <string>

class Shader {
public:
    Shader(const char *vert, 
            const char *tesc,
            const char *tese,
            const char *geom,
            const char *frag,
            const char *comp,
            const std::vector<std::string> &uniforms);
    ~Shader();
    inline void start() { glUseProgram(programId); }
    inline void stop() { glUseProgram(0); }

    inline void setFloat(const char *name, float f)
        { glUniform1f(locations[name], f); }
private:
    void attachShader(const char *path, GLenum type);
    GLuint programId;
    const bool isCompute;
    std::unordered_map<std::string, GLuint> locations;
    std::vector<GLuint> shaders;
};

#endif /* SHADER */
