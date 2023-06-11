#include "Engine.hpp"

Engine::Engine(int width, int height) :
    width(width), height(height)
{
    initialize();
}

Engine::~Engine() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Engine::render() {
    updateDisplay();
}

void Engine::initialize() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window = glfwCreateWindow(width, height, "AfterburnerEngine", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glewInit();
}

void Engine::updateDisplay() {
    double newTime = glfwGetTime();
    deltaTime = newTime - oldTime;
    oldTime = newTime;

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    if (w != width || h != height) {
        width = w; height = h;
        // resize event
    }
    glViewport(0, 0, width, height);
    glfwSwapBuffers(window);
    glfwPollEvents();
}
