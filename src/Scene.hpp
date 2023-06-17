#ifndef SCENE
#define SCENE

#include <glm/glm.hpp>

#include <memory>

#include "Texture.hpp"

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

class Terrain {
public:
    Terrain(const glm::vec3 &position, int numTiles, float tileSize, float tiling,
                Texture *texture, Texture *heightmap, float normalStrength);
    const glm::vec3 position;
    const int numTiles;
    const float tileSize;
    float tiling;
    Texture *texture, *heightmap, *normalmap;
};

class Scene {
public:
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Terrain> terrain;
    glm::vec3 toLightVector;
    Texture *atmosphereTexture;
};

#endif /* SCENE */
