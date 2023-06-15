#include "Engine.hpp"

#include "GLResource.hpp"

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stdout, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

GLWrapper::GLWrapper(int width, int height) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window = glfwCreateWindow(width, height, "AfterburnerEngine", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glewInit();

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
}
GLWrapper::~GLWrapper() {
    GLResource::cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();
}

Engine::Engine(int width, int height) :
   glWrapper(width, height), window(glWrapper.window),
    width(width), height(height), dummyVao(new Vao()),
    terrainShader(
        "res/shader/terrain.vert",
        "res/shader/terrain.tesc",
        "res/shader/terrain.tese",
        nullptr,
        "res/shader/terrain.frag",
        nullptr, {"position", "numTiles", "tileSize", "vpMatrix", "cameraPos", "tiling"}
    ),
    normalCompShader(
        nullptr, nullptr, nullptr, nullptr, nullptr, "res/shader/normals.comp",
        {"N", "strength"}
    ),
    deferredShader(
        "res/shader/deferred.vert", nullptr, nullptr, nullptr, "res/shader/deferred.frag", nullptr,
        {"cameraPos", "invProjMatrix", "invViewMatrix"}
    )
{
    engine = this;
    deferredFbo = new Fbo(width, height, {GL_RGBA32F, GL_RGBA32F}, true);
}

Engine::~Engine() {}

void Engine::render(Scene *scene) {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    deferredFbo->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (isKeyDown(GLFW_KEY_F3)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    renderTerrain(scene);
    deferredFbo->unbind();

    doDeferredShading(scene);

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
        deferredFbo->resize(width, height);
    }
    glViewport(0, 0, width, height);
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Engine::renderTerrain(Scene *scene) {
    Terrain *terrain = scene->terrain.get();
    terrainShader.start();
    terrainShader.setVec3("position", terrain->position);
    terrainShader.setInt("numTiles", terrain->numTiles);
    terrainShader.setFloat("tileSize", terrain->tileSize);
    terrainShader.setMat4("vpMatrix", scene->camera->vpMatrix);
    terrainShader.setVec3("cameraPos", scene->camera->position);
    terrainShader.setFloat("tiling", terrain->tiling);
    terrain->texture->bind(0);
    terrain->heightmap->bind(1);
    terrain->normalmap->bind(2);
    dummyVao->bind();
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawArraysInstanced(GL_PATCHES, 0, 4, terrain->numTiles * terrain->numTiles);
    terrainShader.stop();
}

void Engine::computeNormalMap(Texture *heightmap, Texture *normalmap, float strength) {
    const int N = normalmap->width;
    assert(N == normalmap->height && N % 16 == 0);
    normalCompShader.start();
    normalCompShader.setInt("N", N);
    normalCompShader.setFloat("strength", strength);
    heightmap->bind(0);
    normalmap->bindImage(0);
    normalCompShader.dispatchCompute(N/16, N/16, 1);
    normalCompShader.stop();
}

void Engine::doDeferredShading(Scene *scene) {
    Camera *cam = scene->camera.get();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    deferredShader.start();
    deferredShader.setVec3("cameraPos", cam->position);
    deferredShader.setMat4("invProjMatrix", glm::inverse(cam->projectionMatrix));
    deferredShader.setMat4("invViewMatrix", glm::inverse(cam->viewMatrix));
    deferredFbo->bindColourAttachment(0, 0);
    deferredFbo->bindColourAttachment(1, 1);
    dummyVao->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    deferredShader.stop();
}
