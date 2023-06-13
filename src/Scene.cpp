#include "Scene.hpp"

#include "Engine.hpp"

#include <glm/gtx/transform.hpp>

void Camera::move() {
    float delta = Engine::engine->delta();
    float rot = 30.f * delta;
    float move = 0.f, movey = 0.f;

    if (Engine::engine->isKeyDown(GLFW_KEY_W))
        move = -10.f * delta;
    else if (Engine::engine->isKeyDown(GLFW_KEY_S))
        move = 10.f * delta;

    if (Engine::engine->isKeyDown(GLFW_KEY_SPACE))
        movey = 10.f * delta;
    else if (Engine::engine->isKeyDown(GLFW_KEY_LEFT_SHIFT))
        movey = -10.f * delta;

    position.x += sinf(glm::radians(yaw)) * move;
    position.y += movey;
    position.z += cosf(glm::radians(yaw)) * move;

    if (Engine::engine->isKeyDown(GLFW_KEY_A))
        yaw += rot;
    if (Engine::engine->isKeyDown(GLFW_KEY_D))
        yaw -= rot;
    if (Engine::engine->isKeyDown(GLFW_KEY_J))
        pitch -= rot;
    if (Engine::engine->isKeyDown(GLFW_KEY_K))
        pitch += rot;
}

void Camera::update() {
    viewMatrix = glm::mat4(1.f);
    viewMatrix = glm::translate(viewMatrix, position);
    viewMatrix = glm::rotate(viewMatrix, glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f));
    viewMatrix = glm::rotate(viewMatrix, glm::radians(pitch), glm::vec3(1.f, 0.f, 0.f));
    viewMatrix = glm::rotate(viewMatrix, glm::radians(roll), glm::vec3(0.f, 0.f, 1.f));
    viewMatrix = glm::inverse(viewMatrix);

    float aspect = float(Engine::engine->getDisplayWidth()) / Engine::engine->getDisplayHeight();
    projectionMatrix = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);

    vpMatrix = projectionMatrix * viewMatrix;
}