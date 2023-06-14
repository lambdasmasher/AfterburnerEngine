#include "Engine.hpp"

int main(void) {
    Engine engine(1920, 1080);

    Scene scene;
    scene.camera = std::make_unique<Camera>(glm::vec3(0.f, 100.f, 0.f), 1.f, 5000.f, 50.f);
    scene.terrain = std::make_unique<Terrain>(glm::vec3(-320.f, 0.f, -320.f), 16, 50.f,
                         Texture::loadFromImage("res/ground.png"), 100.f, Texture::loadHeightmap("res/heightmap.png"));

    while (!engine.windowShouldClose()) {
        scene.camera->move();
        scene.camera->update();
        engine.render(&scene);
    }

    return 0;
}
