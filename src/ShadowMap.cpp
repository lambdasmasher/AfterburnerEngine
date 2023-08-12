#include "ShadowMap.hpp"

#include "Engine.hpp"
#include <glm/gtx/transform.hpp>

#define DEPTH_MAP_RESOLUTION 1024

ShadowMap::ShadowMap(const std::vector<float> &cascades) : cascades(cascades) {
    glGenFramebuffers(1, &fboId);
    glGenTextures(1, &texArrayId);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texArrayId);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F,
        DEPTH_MAP_RESOLUTION, DEPTH_MAP_RESOLUTION, cascades.size(),
        0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float border[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, border);

    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texArrayId, 0);
}

ShadowMap::~ShadowMap() {
    glDeleteTextures(1, &texArrayId);
    glDeleteFramebuffers(1, &fboId);
}

void ShadowMap::bindFbo() {
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    glDrawBuffer(GL_NONE);
    glViewport(0, 0, DEPTH_MAP_RESOLUTION, DEPTH_MAP_RESOLUTION);
}
void ShadowMap::unbindFbo() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Engine::engine->getDisplayWidth(), Engine::engine->getDisplayHeight());
}

void ShadowMap::update(Scene *scene) {
    lightVpMats.clear();
    for (unsigned i = 1; i < cascades.size(); i++) {
        float near = cascades[i - 1], far = cascades[i];
        float aspect = float(Engine::engine->getDisplayWidth()) / Engine::engine->getDisplayHeight();
        glm::mat4 proj = glm::perspective(glm::radians(scene->camera->fov), aspect, near, far);
        glm::mat4 invViewProj = glm::inverse(proj * scene->camera->viewMatrix);

        glm::vec3 avg(0.f);
        std::vector<glm::vec3> points;
        float vertices[2] = {-1.0f, 1.0f};
        for (float x : vertices) {
            for (float y : vertices) {
                for (float z : vertices) {
                    glm::vec4 point = invViewProj * glm::vec4(x, y, z, 1.0f);
                    glm::vec3 p = glm::vec3(point) / point.w;
                    avg += p;
                    points.push_back(p);
                }
            }
        }
        avg /= 8.0f;

        glm::mat4 lightView = glm::lookAt(avg + scene->toLightVector, avg, glm::vec3(0.f, 1.f, 0.f));

        glm::vec3 min(1e9), max(-1e9);
        for (glm::vec3 p : points) {
            p = glm::vec3(lightView * glm::vec4(p, 1.0f));
            min = glm::min(min, p);
            max = glm::max(max, p);
        }

        min.z -= 100.f;
        max.z += 100.f;

        glm::mat4 lightProj = glm::ortho(min.x, max.x, min.y, max.y, min.z, max.z);
        lightVpMats.push_back(lightProj * lightView);
    }
}
