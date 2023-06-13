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
    ~Vao() { glDeleteVertexArrays(1, &vaoId); };
    const GLuint vaoId;
    inline void bind() { glBindVertexArray(vaoId); }
};

#endif /* VAO */
