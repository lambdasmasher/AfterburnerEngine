#ifndef FFTWATER
#define FFTWATER

#include <glm/glm.hpp>

#include "Shader.hpp"
#include "Texture.hpp"

class FFTWater {
public:
    FFTWater(int N, float A, float V, float L, glm::vec2 w, float normalStrength);
    void update(float delta);
    Texture * const h0kTexture;
    Texture * const hktY;
    Texture * const hktX;
    Texture * const hktZ;
    Texture * const pong;
    Texture * const butterflyTexture;
    Texture * const displacementMap;
    Texture * const normalMap;
private:
    void computeH0k();
    void computeButterfly();
    void ifftHelper(Texture *ping, Texture *pong);
    glm::vec2 h0k(glm::vec2 k);
    const int N, logN;
    const float A, V, L, normalStrength;
    const glm::vec2 w;
    float t = 0.0f;

    Shader hktShader, ifftShader, combineShader;
};

#endif /* FFTWATER */
