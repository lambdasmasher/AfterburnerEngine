#ifndef VAO
#define VAO

#include <GL/glew.h>
#include "GLResource.hpp"

class Vao : public GLResource {
public:
    Vao() : vaoId([](){
        GLuint id;
        glGenVertexArrays(1, &id);
        return id;
    }()) {};
    ~Vao();
    const GLuint vaoId;
    inline void bind() { glBindVertexArray(vaoId); }
private:
    void addVbo(const std::vector<float> &data, int group);
    std::vector<GLuint> vboIds;
};

#endif /* VAO */
