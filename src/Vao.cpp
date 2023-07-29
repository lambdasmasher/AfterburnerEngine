#include "Vao.hpp"

Vao::~Vao() {
    glDeleteVertexArrays(1, &vaoId);
    glDeleteBuffers(vboIds.size(), vboIds.data());
    glDeleteBuffers(1, &indexBufferId);
}

void Vao::addVbo(const std::vector<float> &data, int group) {
    this->bind();
    GLuint vboId;
    glGenBuffers(1, &vboId);
    unsigned idx = vboIds.size();
    vboIds.push_back(vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(idx, group, GL_FLOAT, GL_FALSE, group * sizeof(float), NULL);
    glEnableVertexAttribArray(idx);
}

void Vao::setIndexBuffer(const std::vector<unsigned> &indices) {
    vertexCount = indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_DRAW);
}
