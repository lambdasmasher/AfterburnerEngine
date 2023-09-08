#include "Scene.hpp"

#include "Engine.hpp"

#include <glm/gtx/transform.hpp>
#include <random>

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

static bool isValid(glm::vec2 candidate, float extent, float cellSize, float radius, int dim,
                    const std::vector<glm::vec2> &points,
                    const std::vector<std::vector<int>> &grid) {
    if (candidate.x > 0.0f && candidate.x < extent && candidate.y > 0.0f && candidate.y < extent) {
        int cellX = (int)(candidate.x / cellSize);
        int cellY = (int)(candidate.y / cellSize);
        for (int i = cellX - 2; i <= cellX + 2; i++) {
            for (int j = cellY - 2; j <= cellY + 2; j++) {
                if (i >= 0 && i < dim && j >= 0 && j < dim) {
                    int pointIndex = grid[i][j] - 1;
                    if (pointIndex != -1) {
                        float dist = glm::distance(candidate, points[pointIndex]);
                        if (dist < radius) {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }
    return false;
}
static std::vector<glm::vec2> poissonDisc(float radius, float extent) {
    float cellSize = radius / sqrtf(2.0f);

    int dim = int(ceilf(extent / cellSize));
    std::vector<std::vector<int>> grid(dim, std::vector<int>(dim, 0));
    std::vector<glm::vec2> points;
    std::vector<glm::vec2> spawnPoints;
    std::mt19937 rng(1337);
    
    spawnPoints.emplace_back(extent / 2.0f, extent / 2.0f);
    while (!spawnPoints.empty()) {
        int spawnIndex =
            std::uniform_int_distribution<int>(0, spawnPoints.size()-1)(rng);
        glm::vec2 spawnCenter = spawnPoints[spawnIndex];
        bool accepted = false;
        for (int i = 0; i < 30; i++) {
            float angle =
                std::uniform_real_distribution<float>(0.0f, 2.0f*M_PIf)(rng);
            glm::vec2 dir(cosf(angle), sinf(angle));
            glm::vec2 candidate = spawnCenter + dir
                * std::uniform_real_distribution<float>(radius, 2.0f*radius)(rng);
            if (isValid(candidate, extent, cellSize, radius, dim, points, grid)) {
                points.push_back(candidate);
                spawnPoints.push_back(candidate);
                grid[(int)(candidate.x/cellSize)][(int)(candidate.y/cellSize)] = points.size();
                accepted = true;
                break;
            }
        }
        if (!accepted) {
            spawnPoints.erase(spawnPoints.begin() + spawnIndex);
        }
    }
    return points;
}

Forest::Forest(Terrain *terrain) {
    trunkModels[0] = std::make_unique<Model>(Vao::fromObj("res/pine.obj", 1)->withInstancedArray(10000), Texture::loadFromImage("res/pine_trunk.jpeg", true));
    leafModels[0] = std::make_unique<Model>(Vao::fromObj("res/pine.obj", 2)->withInstancedArray(10000), Texture::loadFromImage("res/pine_leaves.png", true));

    std::mt19937 rng(42);
    std::uniform_real_distribution<float> rot(0.f, 360.f);
    std::vector<glm::vec2> positions = poissonDisc(20.0f, terrain->tileSize * terrain->numTiles);
    for (glm::vec2 pos : positions) {
        float x = pos.x, z = pos.y;
        float y = terrain->getHeight(x, z);
        if (y < 0.0f) {
            continue;
        }
        trees.emplace_back(
            glm::vec3(x, y, z),
            glm::vec3(0.f, rot(rng), 0.f),
            glm::vec3(3.f)
        );
        trees.back().computeMatrix();
    }
}

void Forest::update(Camera *camera) {
    map.clear();
    for (Entity &tree : trees) {
        trunkModels[0]->mesh->instancedArray->add(&tree);
        leafModels[0]->mesh->instancedArray->add(&tree);
    }
    trunkModels[0]->mesh->instancedArray->flush();
    leafModels[0]->mesh->instancedArray->flush();
    map[trunkModels[0].get()] = trees.size();
    map[leafModels[0].get()] = trees.size();
}
