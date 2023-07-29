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
        nullptr, {"position", "numTiles", "tileSize", "vpMatrix", "clipPlane", "cameraPos", "tiling"}
    ),
    waterShader(
        "res/shader/terrain.vert",
        "res/shader/terrain.tesc",
        "res/shader/water.tese",
        nullptr,
        "res/shader/water.frag",
        nullptr, {"position", "numTiles", "tileSize", "vpMatrix", "cameraPos", "tiling", "nearPlane", "farPlane"}
    ),
    entityShader(
        "res/shader/entity.vert",
        nullptr, nullptr, nullptr,
        "res/shader/entity.frag", nullptr,
        {"modelMatrix", "vpMatrix"}
    ),
    normalCompShader(
        nullptr, nullptr, nullptr, nullptr, nullptr, "res/shader/normals.comp",
        {"N", "strength"}
    ),
    deferredShader(
        "res/shader/deferred.vert", nullptr, nullptr, nullptr, "res/shader/deferred.frag", nullptr,
        {"toLightVector", "cameraPos", "invProjMatrix", "invViewMatrix"}
    )
{
    engine = this;
    deferredFbo = new Fbo(width, height, {GL_RGBA8, GL_RGBA32F, GL_RGBA32F}, true);
    reflectionFbo = new Fbo(width, height, {GL_RGBA8}, false);
    refractionFbo = new Fbo(width, height, {GL_RGBA8}, true);

    macroVariation = Texture::loadFromImage("res/T_MacroVariation.png");
}

Engine::~Engine() {}

void Engine::renderReflection(Scene *scene) {
    float y = scene->camera->position.y;
    float pitch = scene->camera->pitch;
    scene->camera->position.y = scene->fftwater->position.y - scene->camera->position.y;
    scene->camera->pitch = -scene->camera->pitch;
    scene->camera->update();

    deferredFbo->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    clipPlane = glm::vec4(0.0f, 1.0f, 0.0f, -scene->fftwater->position.y + 1.0f);
    renderTerrain(scene);
    reflectionFbo->bind();
    doDeferredShading(scene);
    reflectionFbo->unbind();

    scene->camera->position.y = y;
    scene->camera->pitch = pitch;
    scene->camera->update();
}
void Engine::renderRefraction(Scene *scene) {
    deferredFbo->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    clipPlane = glm::vec4(0.0f, -1.0f, 0.0f, -scene->fftwater->position.y + 2.0f);
    renderTerrain(scene);
    refractionFbo->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    doDeferredShading(scene);
    deferredFbo->blitDepth(refractionFbo);
    refractionFbo->unbind();
}
void Engine::render(Scene *scene) {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);

    glEnable(GL_CLIP_DISTANCE0);
    renderReflection(scene);
    renderRefraction(scene);
    glDisable(GL_CLIP_DISTANCE0);

    deferredFbo->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (isKeyDown(GLFW_KEY_F3)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    renderTerrain(scene);
    renderForest(scene);
    renderWater(scene);
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
        reflectionFbo->resize(width, height);
        refractionFbo->resize(width, height);
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
    terrainShader.setVec4("clipPlane", clipPlane);
    terrainShader.setVec3("cameraPos", scene->camera->position);
    terrainShader.setFloat("tiling", terrain->tiling);
    terrain->texture->bind(0);
    terrain->heightmap->bind(1);
    terrain->normalmap->bind(2);
    macroVariation->bind(3);
    dummyVao->bind();
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawArraysInstanced(GL_PATCHES, 0, 4, terrain->numTiles * terrain->numTiles);
    terrainShader.stop();
}

void Engine::renderWater(Scene *scene) {
    FFTWater *water = scene->fftwater.get();
    water->update(delta() * 5.0f);
    waterShader.start();
    waterShader.setVec3("position", water->position);
    waterShader.setInt("numTiles", water->numTiles);
    waterShader.setFloat("tileSize", water->tileSize);
    waterShader.setMat4("vpMatrix", scene->camera->vpMatrix);
    waterShader.setVec3("cameraPos", scene->camera->position);
    waterShader.setFloat("tiling", water->tiling);
    waterShader.setFloat("nearPlane", scene->camera->nearPlane);
    waterShader.setFloat("farPlane", scene->camera->farPlane);
    water->displacementMap->bind(0);
    water->normalMap->bind(1);
    reflectionFbo->bindColourAttachment(0, 2);
    refractionFbo->bindColourAttachment(0, 3);
    refractionFbo->bindDepthAttachment(4);
    dummyVao->bind();
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawArraysInstanced(GL_PATCHES, 0, 4, water->numTiles * water->numTiles);
    waterShader.stop();
}

void Engine::renderForest(Scene *scene) {
    entityShader.start();
    entityShader.setMat4("vpMatrix", scene->camera->vpMatrix);
    scene->forest->treeModel->bind();
    unsigned vertexCount = scene->forest->treeModel->mesh->getVertexCount();
    for (Entity &tree : scene->forest->trees) {
        tree.computeMatrix();
        entityShader.setMat4("modelMatrix", tree.matrix);
        glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, NULL);
    }
    entityShader.stop();
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
    deferredShader.setVec3("toLightVector", scene->toLightVector);
    deferredShader.setVec3("cameraPos", cam->position);
    deferredShader.setMat4("invProjMatrix", glm::inverse(cam->projectionMatrix));
    deferredShader.setMat4("invViewMatrix", glm::inverse(cam->viewMatrix));
    deferredFbo->bindColourAttachment(0, 0);
    deferredFbo->bindColourAttachment(1, 1);
    deferredFbo->bindColourAttachment(2, 2);
    deferredFbo->bindDepthAttachment(3);
    scene->atmosphereTexture->bind(4);
    refractionFbo->bindColourAttachment(0, 5);
    dummyVao->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    deferredShader.stop();
}
