#include "Engine.hpp"

int main(void) {
    Engine engine(1920, 1080);

    Scene scene;
    scene.camera = std::make_unique<Camera>(glm::vec3(0.f, 100.f, 0.f), 1.f, 5000.f, 50.f);
    scene.terrain = std::make_unique<Terrain>(glm::vec3(-320.f, 0.f, -320.f), 16, 50.f, 100.f,
            Texture::loadFromImage("res/ground.png"), Texture::loadHeightmap("res/heightmap.png"), 15.f);
    scene.toLightVector = glm::vec3(0.f, 1.f, 0.5f);
    scene.atmosphereTexture = Texture::atmosphereCubemap(scene.toLightVector, 512);

    scene.fftwater = std::make_unique<FFTWater>(512, 2.0f, 34.0f, 2000.0f, glm::vec2(1, 1));

    while (!engine.windowShouldClose()) {
        scene.camera->move();
        scene.camera->update();
        engine.render(&scene);
    }

    return 0;
}
