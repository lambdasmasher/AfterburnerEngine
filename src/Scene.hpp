#ifndef SCENE
#define SCENE

#include <glm/glm.hpp>

#include <memory>

class Camera {
public:
    Camera(const glm::vec3 &position, float nearPlane, float farPlane, float fov)
        : position(position), nearPlane(nearPlane), farPlane(farPlane), fov(fov) { update(); }
    glm::vec3 position;
    float pitch = 0.f, yaw = 0.f, roll = 0.f;
    float nearPlane, farPlane, fov;
    glm::mat4 viewMatrix, projectionMatrix, vpMatrix;
    void move();
    void update();
};

class Scene {
public:
    std::unique_ptr<Camera> camera;
};

#endif /* SCENE */
