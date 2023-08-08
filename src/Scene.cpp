#include "Scene.hpp"

#include "Engine.hpp"

#include <glm/gtx/transform.hpp>

void Camera::move() {
    float delta = Engine::engine->delta();
    float rot = 60.f * delta;
    float move = 0.f, movey = 0.f;

    if (Engine::engine->isKeyDown(GLFW_KEY_W))
        move = -50.f * delta;
    else if (Engine::engine->isKeyDown(GLFW_KEY_S))
        move = 50.f * delta;

    if (Engine::engine->isKeyDown(GLFW_KEY_SPACE))
        movey = 50.f * delta;
    else if (Engine::engine->isKeyDown(GLFW_KEY_LEFT_SHIFT))
        movey = -50.f * delta;

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

Terrain::Terrain(const glm::vec3 &position, int numTiles, float tileSize, float tiling,
                Texture *texture, const char *heightpath, float amplitude, float normalStrength)
        : position(position), numTiles(numTiles), tileSize(tileSize), tiling(tiling),
          amplitude(amplitude), texture(texture)
{
    heightmap = Texture::loadHeightmap(heightpath, heights);
    normalmap = Texture::texStorage(heightmap->width, heightmap->height);
    Engine::engine->computeNormalMap(heightmap, normalmap, normalStrength);
}

static float lerp(float a, float b, float t) {
    return a + t * (b - a);
}
float Terrain::sampleHeight(unsigned i) {
    if (i < 0 || i >= heights.size())
        return 0.f;
    return (heights[i] / 65535.0f - 0.5f) * 2.0f * amplitude;
}
float Terrain::getHeight(float x, float z) {
    int N = heightmap->width;
    float u = (x - position.x) / (tileSize * numTiles) * N;
    float v = (z - position.z) / (tileSize * numTiles) * N;
    float a = sampleHeight(int(floor(u)) + N*int(floor(v)));
    float b = sampleHeight(int(floor(u)) + N*int(ceil(v)));
    float c = sampleHeight(int(ceil(u)) + N*int(floor(v)));
    float d = sampleHeight(int(ceil(u)) + N*int(ceil(v)));
    float f1 = lerp(a, b, modff(v, &x));
    float f2 = lerp(c, d, modff(v, &x));
    return lerp(f1, f2, modff(u, &x));
}

void Entity::computeMatrix() {
    matrix = glm::mat4(1.f);
    matrix = glm::translate(matrix, position);
    matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
    matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
    matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
    matrix = glm::scale(matrix, scale);
}

#include <random>
Forest::Forest(Terrain *terrain) {
    treeArray = new InstancedArray(1000);
    trunkModels[0] = std::make_unique<Model>(Vao::fromObj("res/tree.obj")->attachInstancedArray(treeArray), Texture::loadFromImage("res/tree.png"));

    std::mt19937 rng(42);
    std::uniform_real_distribution<float> pos(-200.f, 200.f);
    std::uniform_real_distribution<float> rot(0.f, 360.f);
    for (int i = 0; i < 1; i++) {
        glm::vec3 position(pos(rng), 0.f, pos(rng));
        position.y = terrain->getHeight(position.x, position.z);
        map[trunkModels[0].get()].push_back(new Entity(
            position,
            glm::vec3(0.f, rot(rng), 0.f),
            glm::vec3(1.f)
        ));
        map[trunkModels[0].get()].back()->computeMatrix();
    }
}
