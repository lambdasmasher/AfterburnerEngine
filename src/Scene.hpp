#ifndef SCENE
#define SCENE

#include <glm/glm.hpp>

#include <memory>

#include "Texture.hpp"
#include "FFTWater.hpp"
#include "Vao.hpp"
#include "InstancedArray.hpp"
#include "ShadowMap.hpp"

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
                Texture *texture, const char *heightpath, float amplitude, float normalStrength);
    const glm::vec3 position;
    const int numTiles;
    const float tileSize;
    float tiling;
    float amplitude;
    Texture *texture, *heightmap, *normalmap;
    float getHeight(float x, float z);
private:
    float sampleHeight(unsigned i);
    std::vector<uint16_t> heights;
};

struct Model {
    Model(Vao *mesh, Texture *texture)
        : mesh(mesh), texture(texture) {}
    Vao * const mesh;
    Texture * const texture;
};
struct Entity {
    Entity(const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl)
        : position(pos), rotation(rot), scale(scl) {}
    glm::vec3 position, rotation, scale;
    glm::mat4 matrix;
    void computeMatrix();
};

class Forest {
    friend class Engine;
public:
    Forest(Terrain *terrain);
    void update(Camera *camera);
    std::unique_ptr<Model> trunkModels[3], leafModels[3];
private:
    std::vector<Entity> trees;
    std::unordered_map<Model*, unsigned> map;
};

class Scene {
public:
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Terrain> terrain;
    std::unique_ptr<FFTWater> fftwater;
    std::unique_ptr<Forest> forest;
    std::unique_ptr<ShadowMap> shadowMap;
    glm::vec3 toLightVector;
    Texture *atmosphereTexture;
    float fogDensity, fogGradient;
};

#endif /* SCENE */
