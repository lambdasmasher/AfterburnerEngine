#ifndef TEXTURE
#define TEXTURE

#include <GL/glew.h>

#include "GLResource.hpp"

class Texture : public GLResource {
public:
    static Texture* loadFromImage(const char *path);
    static Texture* loadHeightmap(const char *path);
    static Texture* texStorage(int width, int height);
    inline void bind(unsigned i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(texType, texId);
    }
    inline void bindImage(unsigned i)
        { glBindImageTexture(i, texId, 0, false, 0, GL_READ_WRITE, GL_RGBA32F); }
    const int width, height;
private:
    Texture(GLenum texType, int width, int height) : texId([](){
        GLuint id; glGenTextures(1, &id); return id;
    }()), texType(texType), width(width), height(height) {}
    const GLuint texId;
    const GLenum texType;
protected:
    ~Texture() { glDeleteTextures(1, &texId); }
};

#endif /* TEXTURE */
