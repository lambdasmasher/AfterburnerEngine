#ifndef INSTANCEDARRAY
#define INSTANCEDARRAY

#include "GLResource.hpp"
#include "Vao.hpp"

class Entity;

class InstancedArray : public GLResource {
public:
    InstancedArray(unsigned numInstances);
    ~InstancedArray() { glDeleteBuffers(1, &vboId); }
    void setupAttributes(Vao *vao, unsigned start);
    void add(Entity *entity);
    void flush();
private:
    GLuint vboId;
    unsigned ptr = 0;
    std::vector<float> data;
};

#endif /* INSTANCEDARRAY */
