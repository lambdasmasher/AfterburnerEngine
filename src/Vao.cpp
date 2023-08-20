#include "Vao.hpp"

#include <map>
#include <glm/glm.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "InstancedArray.hpp"

Vao::~Vao() {
    glDeleteVertexArrays(1, &vaoId);
    glDeleteBuffers(vboIds.size(), vboIds.data());
    glDeleteBuffers(1, &indexBufferId);
}

Vao* Vao::withInstancedArray(unsigned numInstances) {
    instancedArray.reset(new InstancedArray(numInstances, this, vboIds.size()));
    return this;
}

struct Vertex {
    glm::vec3 position;
    glm::vec2 texcoord;
    glm::vec3 normal;
    bool operator<(const Vertex &that) const {
        return memcmp(this, &that, sizeof(Vertex)) < 0;
    }
};
Vao* Vao::fromObj(const char *file, unsigned shapeMask) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, file)) {
        printf("failed to load obj file '%s'\n", file);
        puts(err.c_str());
        exit(0);
    }

    std::map<Vertex, unsigned> vertices; 
    std::vector<unsigned> indices;
    std::vector<float> positions, texcoords, normals;
    for (unsigned s = 0; s < shapes.size(); s++) {
        if (!((shapeMask >> s) & 1)) {
            continue;
        }
        const auto &shape = shapes[s];
        for (const auto &index : shape.mesh.indices) {
            Vertex v;
            for (unsigned i = 0; i < 3; i++) {
                v.position[i] = attrib.vertices[3 * index.vertex_index + i];
                v.normal[i] = attrib.normals[3 * index.normal_index + i];
            }
            v.texcoord.x = attrib.texcoords[2 * index.texcoord_index + 0];
            v.texcoord.y = attrib.texcoords[2 * index.texcoord_index + 1];

            if (!vertices.count(v)) {
                vertices[v] = vertices.size();
                for (unsigned i = 0; i < 3; i++) {
                    positions.push_back(v.position[i]);
                    if (i < 2)
                        texcoords.push_back(v.texcoord[i]);
                    normals.push_back(v.normal[i]);
                }
            }
            indices.push_back(vertices[v]);
        }
    }

    Vao *vao = new Vao();
    vao->setIndexBuffer(indices);
    vao->addVbo(positions, 3);
    vao->addVbo(texcoords, 2);
    vao->addVbo(normals, 3);
    return vao;
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
