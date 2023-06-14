#ifndef TEXTURE
#define TEXTURE

#include <GL/glew.h>

#include "GLResource.hpp"

class Texture : public GLResource {
public:
    static Texture* loadFromImage(const char *path);
    inline void bind(unsigned i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(texType, texId);
    }
private:
    Texture(GLenum texType) : texId([](){
        GLuint id; glGenTextures(1, &id); return id;
    }()), texType(texType) {}
    const GLuint texId;
    const GLenum texType;
protected:
    ~Texture() { glDeleteTextures(1, &texId); }
};

#endif /* TEXTURE */
