#include "Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture* Texture::loadFromImage(const char *path) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &width, &height, &channels, 3);
    if (data == NULL) {
        printf("failed to load image %s\n", path);
        exit(0);
    }

    Texture *texture = new Texture(GL_TEXTURE_2D);
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

Texture* Texture::loadHeightmap(const char *path) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned short *data = stbi_load_16(path, &width, &height, &channels, 1);
    if (data == NULL) {
        printf("failed to load image %s\n", path);
        exit(0);
    }

    Texture *texture = new Texture(GL_TEXTURE_2D);
    texture->bind(0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, width, height, 0, GL_RED, GL_UNSIGNED_SHORT, data);

    stbi_image_free(data);
    return texture;
}
