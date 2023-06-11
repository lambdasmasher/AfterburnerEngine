#include "Engine.hpp"
#include <iostream>

int main(void) {
    Engine engine(1920, 1080);

    float time = 0.f;
    while (!engine.windowShouldClose()) {
        glClearColor(1, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        engine.render();
        time += engine.delta();
        std::cout << time << "\n";
    }

    return 0;
}
