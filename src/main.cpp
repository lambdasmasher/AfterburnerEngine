#include "Engine.hpp"

#include <stdio.h>

int main(void) {
    Engine engine(1920, 1080);

    Scene scene;
    scene.camera = std::make_unique<Camera>(glm::vec3(800.f, 100.f, 800.f), 1.f, 5000.f, 50.f);
    scene.terrain = std::make_unique<Terrain>(glm::vec3(0.f, 0.f, 0.f), 32, 50.f, 100.f,
            Texture::loadFromImage("res/ground.png"), "res/heightmap.bmp", 100.f, 15.f);
    scene.toLightVector = glm::vec3(0.f, 1.f, -0.5f);
    scene.atmosphereTexture = Texture::atmosphereCubemap(scene.toLightVector, 512);
    scene.fogDensity = 0.00025f;
    scene.fogGradient = 4.0f;

    scene.fftwater = std::make_unique<FFTWater>(512, 1.25f, 38.0f, 2000.0f, glm::vec2(1, 1), 1.7f);
    scene.fftwater->position = glm::vec3(-6400.f, 0.f, -6400.f);
    scene.fftwater->numTiles = 256;
    scene.fftwater->tileSize = 50.f;
    scene.fftwater->tiling = 160.f;

    scene.forest = std::make_unique<Forest>(scene.terrain.get());
    scene.shadowMap = std::make_unique<ShadowMap>(std::vector<float>{1.0f, 100.f, 300.f, 1000.f, 2000.f, 5000.f});

    // entire island view
    // scene.camera->position = glm::vec3(1898.41785, 423.51843, 1326.46765);
    // scene.camera->pitch = -26.32413; scene.camera->yaw = 57.87508;

    // more bay
    // scene.camera->position = glm::vec3(1024.35535, 119.95562, 753.73212);
    // scene.camera->pitch = -17.06867; scene.camera->yaw = 55.45488;

    // more valley
    // scene.camera->position = glm::vec3(1030.44836, 186.30179, 743.81372);
    // scene.camera->pitch = -17.06867; scene.camera->yaw = 57.63961;

    // balance
    // scene.camera->position = glm::vec3(1007.35303, 151.30853, 729.17944);
    // scene.camera->pitch = -14.83500; scene.camera->yaw = 57.63961;

    // forest view
    // scene.camera->position = glm::vec3(725.16962, 63.12212, 975.39551);
    // scene.camera->pitch = -11.03120; scene.camera->yaw = -8.99225;

    while (!engine.windowShouldClose()) {
        if (engine.isKeyDown(GLFW_KEY_C)) {
            printf("scene.camera->position = glm::vec3(%.5f, %.5f, %.5f);\n",
                scene.camera->position.x, scene.camera->position.y, scene.camera->position.z);
            printf("scene.camera->pitch = %.5f; scene.camera->yaw = %.5f;\n",
                scene.camera->pitch, scene.camera->yaw);
        }
        scene.camera->move();
        scene.camera->update();
        engine.render(&scene);
    }

    return 0;
}
