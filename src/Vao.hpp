#ifndef VAO
#define VAO

#include <GL/glew.h>
#include "GLResource.hpp"
#include "InstancedArray.hpp"

class Vao : public GLResource {
public:
    Vao() : vaoId([](){
        GLuint id;
        glGenVertexArrays(1, &id);
        return id;
    }()) { glGenBuffers(1, &indexBufferId); };
    ~Vao();
    const GLuint vaoId;
    inline void bind() { glBindVertexArray(vaoId); }
    inline void bindWithIndices() { bind(); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId); }
    inline unsigned getVertexCount() { return vertexCount; }

    Vao* attachInstancedArray(InstancedArray *array);
    static Vao* fromObj(const char *file);
private:
    void addVbo(const std::vector<float> &data, int group);
    void setIndexBuffer(const std::vector<unsigned> &indices);
    GLuint indexBufferId;
    std::vector<GLuint> vboIds;
    unsigned vertexCount;
};

#endif /* VAO */
