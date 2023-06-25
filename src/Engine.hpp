#ifndef ENGINE
#define ENGINE

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Scene.hpp"
#include "Texture.hpp"
#include "Vao.hpp"
#include "Fbo.hpp"
#include "Shader.hpp"

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
    void render(Scene *scene);
    static inline Engine *engine = nullptr;

private:
    GLWrapper glWrapper;
    GLFWwindow *window;
    int width, height;
    double oldTime = 0.0;
    float deltaTime;
    void updateDisplay();
public:
    inline int getDisplayWidth() { return width; }
    inline int getDisplayHeight() { return height; }
    inline bool windowShouldClose() { return glfwWindowShouldClose(window); }
    inline float delta() { return deltaTime; }
    inline bool isKeyDown(int key) { return glfwGetKey(window, key) == GLFW_PRESS; }

private:
    Vao * const dummyVao;
    void renderTerrain(Scene *scene);
    void renderWater(Scene *scene);

public:
    void computeNormalMap(Texture *heightmap, Texture *normalmap, float strength);

private:
    Fbo *deferredFbo;
    void doDeferredShading(Scene *scene);

private:
    Shader terrainShader;
    Shader waterShader;
    Shader normalCompShader;
    Shader deferredShader;
};

#endif /* ENGINE */
