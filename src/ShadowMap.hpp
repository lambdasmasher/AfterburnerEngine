#ifndef SHADOWMAP
#define SHADOWMAP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class Scene;

class ShadowMap {
public:
    ShadowMap(const std::vector<float> &cascades);
    ~ShadowMap();
    void bindFbo();
    void unbindFbo();
    inline void bindMapTexture(unsigned i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texArrayId);
    }
    void update(Scene *scene);
    inline float* getMatrixValPtr() { return (float*)lightVpMats.data(); }
    const std::vector<float> cascades;
private:
    std::vector<glm::mat4> lightVpMats;
    GLuint fboId;
    GLuint texArrayId;
};

#endif /* SHADOWMAP */
