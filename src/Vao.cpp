#include "Vao.hpp"

Vao::~Vao() {
    glDeleteVertexArrays(1, &vaoId);
    glDeleteBuffers(vboIds.size(), vboIds.data());
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
