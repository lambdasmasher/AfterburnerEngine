#include "Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <functional>

Texture* Texture::solidColour(float r, float g, float b, float a) {
    Texture *texture = new Texture(GL_TEXTURE_2D, 1, 1);
    texture->bind(0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    float data[4] = {r, g, b, a};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 1, 1, 0, GL_RGBA, GL_FLOAT, data);
    return texture;
}

Texture* Texture::loadFromImage(const char *path) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &width, &height, &channels, 3);
    if (data == NULL) {
        printf("failed to load image %s\n", path);
        exit(0);
    }

    Texture *texture = new Texture(GL_TEXTURE_2D, width, height);
    texture->bind(0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return texture;
}

Texture* Texture::loadHeightmap(const char *path, std::vector<uint16_t> &heights) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned short *data = stbi_load_16(path, &width, &height, &channels, 1);
    if (data == NULL) {
        printf("failed to load image %s\n", path);
        exit(0);
    }

    assert(width == height);
    Texture *texture = new Texture(GL_TEXTURE_2D, width, height);
    texture->bind(0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, width, height, 0, GL_RED, GL_UNSIGNED_SHORT, data);

    heights.assign(data, data + (width * height));
    stbi_image_free(data);
    return texture;
}

Texture* Texture::texStorage(int width, int height) {
    Texture *texture = new Texture(GL_TEXTURE_2D, width, height);
    texture->bind(0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
    return texture;
}

Texture *Texture::dynamicMipmap(int N) {
    Texture *texture = new Texture(GL_TEXTURE_2D, N, N);
    texture->bind(0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, N, N, 0, GL_RGBA, GL_FLOAT, nullptr);
    return texture;
}

extern glm::vec3 computeAtmosphereColour(glm::vec3 toLightVector, glm::vec3 ray);

Texture* Texture::atmosphereCubemap(glm::vec3 toLightVector, int N) {
    Texture *texture = new Texture(GL_TEXTURE_CUBE_MAP, N, N);
    texture->bind(0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    GLenum sides[6] = {
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    };
    using vec3 = glm::vec3;
    std::function<vec3(float, float)> funcs[6] = {
        [](float i, float j){return vec3(-1, -j, i);}, [](float i, float j){return vec3(1, -j, -i);},
        [](float i, float j){return vec3(i, -1, -j);}, [](float i, float j){return vec3(i, 1, j);},
        [](float i, float j){return vec3(i, -j, -1);}, [](float i, float j){return vec3(i, -j, 1);}
    };
    std::vector<float> data(N * N * 3);
    for (int s = 0; s < 6; s++) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                float a = (float(i) / (N - 1) - 0.5f) * 2.0f;
                float b = (float(j) / (N - 1) - 0.5f) * 2.0f;
                glm::vec3 colour = computeAtmosphereColour(toLightVector, funcs[s](a, b));
                data[3*(j*N+i) + 0] = colour.x;
                data[3*(j*N+i) + 1] = colour.y;
                data[3*(j*N+i) + 2] = colour.z;
            }
        }
        glTexImage2D(sides[s], 0, GL_RGB, N, N, 0, GL_RGB, GL_FLOAT, data.data());
    }

    return texture;
}
