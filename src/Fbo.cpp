#include "Fbo.hpp"

Fbo::Fbo(int width, int height, const std::vector<GLenum> &attachments, const bool hasDepth)
            : width(width), height(height), attachments(attachments), hasDepth(hasDepth) {
    glGenFramebuffers(1, &fboId);
    setup();
}

void Fbo::setup() {
    bind();
    for (unsigned i = 0; i < attachments.size(); i++) {
        GLenum format = attachments[i];
        GLuint texId;
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texId, 0);
        texIds.push_back(texId);
    }
    if (hasDepth) {
        GLuint texId;
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT24, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texId, 0);
        texIds.push_back(texId);
    }
}

void Fbo::cleanup() {
    glDeleteTextures(texIds.size(), texIds.data());
    texIds.clear();
}
