#include "Engine.hpp"

int main(void) {
    Engine engine(1920, 1080);

    Scene scene;
    scene.camera = std::make_unique<Camera>(glm::vec3(800.f, 100.f, 800.f), 1.f, 5000.f, 50.f);
    scene.terrain = std::make_unique<Terrain>(glm::vec3(0.f, 0.f, 0.f), 32, 50.f, 100.f,
            Texture::loadFromImage("res/ground.png"), "res/heightmap.bmp", 100.f, 15.f);
    scene.toLightVector = glm::vec3(0.f, 1.f, 0.5f);
    scene.atmosphereTexture = Texture::atmosphereCubemap(scene.toLightVector, 512);
    scene.fogDensity = 0.00025f;
    scene.fogGradient = 4.0f;

    scene.fftwater = std::make_unique<FFTWater>(512, 1.25f, 38.0f, 2000.0f, glm::vec2(1, 1), 1.7f);
    scene.fftwater->position = glm::vec3(-6400.f, 0.f, -6400.f);
    scene.fftwater->numTiles = 256;
    scene.fftwater->tileSize = 50.f;
    scene.fftwater->tiling = 160.f;

    scene.forest = std::make_unique<Forest>(scene.terrain.get());
    scene.shadowMap = std::make_unique<ShadowMap>(std::vector<float>{1.0f, 300.f, 1000.f, 5000.f});

    // ocean
    // scene.camera->position = glm::vec3(481.821045, 52.152992, 371.357758);
    // scene.camera->pitch = -36.282;
    // scene.camera->yaw = -119.2336;

    // lake
    // scene.camera->position = glm::vec3(165.735336, 11.286901, 135.532547);
    // scene.camera->pitch = -17.967747;
    // scene.camera->yaw = -485.757751;

    while (!engine.windowShouldClose()) {
        scene.camera->move();
        scene.camera->update();
        engine.render(&scene);
    }

    return 0;
}
