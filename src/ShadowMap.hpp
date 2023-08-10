#ifndef SHADOWMAP
#define SHADOWMAP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class Scene;

class ShadowMap {
public:
    ShadowMap(int cascades);
    ~ShadowMap();
    inline void bindFbo() { glBindFramebuffer(GL_FRAMEBUFFER, fboId); }
    inline void bindMapTexture(unsigned i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texArrayId);
    }
    void update(Scene *scene);
private:
    const int cascades;
    std::vector<glm::mat4> lightVpMats;
    GLuint fboId;
    GLuint texArrayId;
};

#endif /* SHADOWMAP */
