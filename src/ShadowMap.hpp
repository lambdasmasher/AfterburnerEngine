#ifndef SHADOWMAP
#define SHADOWMAP

#include "Scene.hpp"

class ShadowMap {
public:
    ShadowMap(int cascades);
    ~ShadowMap();
private:
    const int cascades;
    GLuint fboId;
    GLuint texArrayId;
};

#endif /* SHADOWMAP */
