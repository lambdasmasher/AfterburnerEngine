#include "Engine.hpp"

int main(void) {
    Engine engine(1920, 1080);

    while (!engine.windowShouldClose()) {
        glClearColor(1, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        engine.render();
    }

    return 0;
}
