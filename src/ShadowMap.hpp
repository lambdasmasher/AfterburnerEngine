#ifndef SHADOWMAP
#define SHADOWMAP

#include "Scene.hpp"

class ShadowMap {
public:
    ShadowMap(int cascades);
    ~ShadowMap();
    inline void bindFbo() { glBindFramebuffer(GL_FRAMEBUFFER, fboId); }
    inline void bindMapTexture(unsigned i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texArrayId);
    }
private:
    const int cascades;
    GLuint fboId;
    GLuint texArrayId;
};

#endif /* SHADOWMAP */
