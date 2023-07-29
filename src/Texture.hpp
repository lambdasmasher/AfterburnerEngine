#ifndef TEXTURE
#define TEXTURE

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "GLResource.hpp"

class Texture : public GLResource {
public:
    Texture(GLenum texType, int width, int height) : texId([](){
        GLuint id; glGenTextures(1, &id); return id;
    }()), texType(texType), width(width), height(height) {}
    static Texture* solidColour(float r, float g, float b, float a);
    static Texture* loadFromImage(const char *path);
    static Texture* loadHeightmap(const char *path);
    static Texture* texStorage(int width, int height);
    static Texture* dynamicMipmap(int N);
    static Texture* atmosphereCubemap(glm::vec3 toLightVector, int N);
    inline void bind(unsigned i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(texType, texId);
    }
    inline void bindImage(unsigned i)
        { glBindImageTexture(i, texId, 0, false, 0, GL_READ_WRITE, GL_RGBA32F); }
    const int width, height;
    const GLuint texId;
    const GLenum texType;
protected:
    ~Texture() { glDeleteTextures(1, &texId); }
};

#endif /* TEXTURE */
