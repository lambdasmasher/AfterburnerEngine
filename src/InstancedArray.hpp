#ifndef INSTANCEDARRAY
#define INSTANCEDARRAY

#include "Vao.hpp"

class Entity;

class InstancedArray {
public:
    InstancedArray(unsigned numInstances, Vao *vao, unsigned start);
    ~InstancedArray() { glDeleteBuffers(1, &vboId); }
    void add(Entity *entity);
    void flush();
private:
    GLuint vboId;
    unsigned ptr = 0;
    std::vector<float> data;
};

#endif /* INSTANCEDARRAY */
