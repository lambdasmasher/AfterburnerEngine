#include "Engine.hpp"

#include "GLResource.hpp"

GLWrapper::GLWrapper(int width, int height) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window = glfwCreateWindow(width, height, "AfterburnerEngine", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glewInit();
}
GLWrapper::~GLWrapper() {
    GLResource::cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();
}

Engine::Engine(int width, int height) :
   glWrapper(width, height), window(glWrapper.window),
    width(width), height(height) { engine = this; }

Engine::~Engine() {}

void Engine::render() {
    updateDisplay();
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
