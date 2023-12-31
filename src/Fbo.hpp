#ifndef FBO
#define FBO

#include <GL/glew.h>
#include <vector>
#include "GLResource.hpp"

class Fbo : public GLResource {
public:
    Fbo(int width, int height, const std::vector<GLenum> &attachments, bool hasDepth);
    ~Fbo() { cleanup(); glDeleteFramebuffers(1, &fboId); }
    void resize(int width, int height);
    void bind();
    inline void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
    inline void bindColourAttachment(unsigned attach, unsigned tex) {
        glActiveTexture(GL_TEXTURE0 + tex);
        glBindTexture(GL_TEXTURE_2D, texIds[attach]);
    }
    inline void bindDepthAttachment(unsigned tex) {
        glActiveTexture(GL_TEXTURE0 + tex);
        glBindTexture(GL_TEXTURE_2D, texIds.back());
    }
    inline void blitDepth(Fbo *target) {
        glBlitNamedFramebuffer(fboId, target->fboId, 0, 0, width, height,
            0, 0, target->width, target->height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }
private:
    void setup();
    void cleanup();
    GLuint fboId;
    int width, height;
    std::vector<GLenum> attachments;
    const bool hasDepth;
    std::vector<GLuint> texIds;
};

#endif /* FBO */
