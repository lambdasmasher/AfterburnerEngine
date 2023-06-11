#ifndef ENGINE
#define ENGINE

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GLWrapper {
public:
    GLWrapper(int width, int height);
    ~GLWrapper();
    GLFWwindow *window;
};

class Engine {
public:
    Engine(int width, int height);
    ~Engine();
    void render();

private:
    GLWrapper glWrapper;
    GLFWwindow *window;
    int width, height;
    double oldTime = 0.0;
    float deltaTime;
    void updateDisplay();
public:
    inline bool windowShouldClose() { return glfwWindowShouldClose(window); }
    inline float delta() { return deltaTime; }


};

#endif /* ENGINE */
