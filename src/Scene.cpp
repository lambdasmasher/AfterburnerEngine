#include "Scene.hpp"

#include "Engine.hpp"

#include <glm/gtx/transform.hpp>

void Camera::move() {
    update();
}

void Camera::update() {
    viewMatrix = glm::translate(glm::mat4(1.f), position);
    viewMatrix = glm::rotate(viewMatrix, pitch, glm::vec3(1.f, 0.f, 0.f));
    viewMatrix = glm::rotate(viewMatrix, yaw, glm::vec3(0.f, 1.f, 0.f));
    viewMatrix = glm::rotate(viewMatrix, roll, glm::vec3(0.f, 0.f, 1.f));
    viewMatrix = glm::inverse(viewMatrix);

    float aspect = float(Engine::engine->getDisplayWidth()) / Engine::engine->getDisplayHeight();
    projectionMatrix = glm::perspective(fov, aspect, nearPlane, farPlane);

    vpMatrix = viewMatrix * projectionMatrix;
}