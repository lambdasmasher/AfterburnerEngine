#ifndef VAO
#define VAO

#include <GL/glew.h>
#include <memory>
#include "GLResource.hpp"

class InstancedArray;

class Vao : public GLResource {
public:
    Vao() : vaoId([](){
        GLuint id;
        glGenVertexArrays(1, &id);
        return id;
    }()) { glGenBuffers(1, &indexBufferId); };
    ~Vao();
    const GLuint vaoId;
    std::unique_ptr<InstancedArray> instancedArray;
    inline void bind() { glBindVertexArray(vaoId); }
    inline void bindWithIndices() { bind(); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId); }
    inline unsigned getVertexCount() { return vertexCount; }

    Vao* withInstancedArray(unsigned numInstances);
    static Vao* fromObj(const char *file, unsigned shapeMask = -1U);
private:
    void addVbo(const std::vector<float> &data, int group);
    void setIndexBuffer(const std::vector<unsigned> &indices);
    GLuint indexBufferId;
    std::vector<GLuint> vboIds;
    unsigned vertexCount;
};

#endif /* VAO */
