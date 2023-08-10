#ifndef SHADER
#define SHADER

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
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
    inline void dispatchCompute(int x, int y, int z) {
        assert(isCompute);
        glDispatchCompute(x, y, z);
        glFinish();
    }

    inline void setFloat(const char *name, float f)
        { glUniform1f(locations[name], f); }
    inline void setVec3(const char *name, const glm::vec3 &v)
        { glUniform3f(locations[name], v.x, v.y, v.z); }
    inline void setVec4(const char *name, const glm::vec4 &v)
        { glUniform4f(locations[name], v.x, v.y, v.z, v.w); }
    inline void setMat4(const char *name, const glm::mat4 &m)
        { glUniformMatrix4fv(locations[name], 1, GL_FALSE, glm::value_ptr(m)); }
    inline void setMat4Array(const char *name, float *val, int cnt)
        { glUniformMatrix4fv(locations[name], cnt, GL_FALSE, val); }
    inline void setInt(const char *name, int i)
        { glUniform1i(locations[name], i); }
    
private:
    void attachShader(const char *path, GLenum type);
    GLuint programId;
    const bool isCompute;
    std::unordered_map<std::string, GLuint> locations;
    std::vector<GLuint> shaders;
};

#endif /* SHADER */
