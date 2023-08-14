#include "InstancedArray.hpp"

#include "Scene.hpp"

#define FLOATS_PER_INSTANCE 16
#define BYTES_PER_INSTANCE (sizeof(float) * FLOATS_PER_INSTANCE)

InstancedArray::InstancedArray(unsigned numInstances, Vao *vao, unsigned start) {
    glGenBuffers(1, &vboId);
    data.resize(FLOATS_PER_INSTANCE * numInstances);

    unsigned attribs[] = {4, 4, 4, 4};
    vao->bind();
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    unsigned idx = start, offset = 0;
    for (unsigned size : attribs) {
        glEnableVertexAttribArray(idx);
        glVertexAttribPointer(idx, size, GL_FLOAT, false, BYTES_PER_INSTANCE, (void*)(sizeof(float) * offset));
        glVertexAttribDivisor(idx, 1);
        idx++;
        offset += size;
    }
}

void InstancedArray::add(Entity *entity) {
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            data[ptr++] = entity->matrix[r][c];
}

void InstancedArray::flush() {
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, ptr * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
    ptr = 0;
}
