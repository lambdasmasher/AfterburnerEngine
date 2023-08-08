#include "ShadowMap.hpp"

#define DEPTH_MAP_RESOLUTION 1024

ShadowMap::ShadowMap(int cascades) : cascades(cascades) {
    glGenFramebuffers(1, &fboId);
    glGenTextures(1, &texArrayId);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texArrayId);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F,
        DEPTH_MAP_RESOLUTION, DEPTH_MAP_RESOLUTION, cascades + 1,
        0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float border[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, border);

    glNamedFramebufferTexture(fboId, GL_DEPTH_ATTACHMENT, texArrayId, 0);
}

ShadowMap::~ShadowMap() {
    glDeleteTextures(1, &texArrayId);
    glDeleteFramebuffers(1, &fboId);
}
