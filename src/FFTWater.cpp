#include "FFTWater.hpp"

#include "Engine.hpp"

#include <random>
#include <time.h>

using namespace glm;

FFTWater::FFTWater(int N, float A, float V, float L, glm::vec2 w, float normalStrength)
    : N(N), logN(int(std::log2(N))), A(A), V(V), L(L), normalStrength(normalStrength), w(w),
    h0kTexture(new Texture(GL_TEXTURE_2D, N, N)), hktY(Texture::texStorage(N, N)), hktX(Texture::texStorage(N, N)),
    hktZ(Texture::texStorage(N, N)), pong(Texture::texStorage(N, N)), butterflyTexture(new Texture(GL_TEXTURE_2D, logN, N)),
    displacementMap(Texture::texStorage(N, N)), normalMap(Texture::texStorage(N, N)),
    hktShader(
        nullptr, nullptr, nullptr, nullptr, nullptr, "res/shader/hkt.comp",
        {"N", "L", "t"}
    ),
    ifftShader(
        nullptr, nullptr, nullptr, nullptr, nullptr, "res/shader/ifft.comp",
        {"stage", "vertical"}
    ),
    combineShader(
        nullptr, nullptr, nullptr, nullptr, nullptr, "res/shader/combine.comp",
        {"N"}
    )
{
    assert((N & (N - 1)) == 0);
    computeH0k();
    computeButterfly();
}

void FFTWater::ifftHelper(Texture *ping, Texture *pong) {
    ifftShader.setInt("vertical", 0);
    for (int i = 0; i < logN; i++) {
        ifftShader.setInt("stage", i);
        ping->bindImage(1);
        pong->bindImage(2);
        ifftShader.dispatchCompute(N/16, N/16, 1);
        std::swap(ping, pong);
    }
    ifftShader.setInt("vertical", 1);
    for (int i = 0; i < logN; i++) {
        ifftShader.setInt("stage", i);
        ping->bindImage(1);
        pong->bindImage(2);
        ifftShader.dispatchCompute(N/16, N/16, 1);
        std::swap(ping, pong);
    }
}
void FFTWater::update(float delta) {
    t += delta;
    hktShader.start();
    hktShader.setInt("N", N);
    hktShader.setFloat("L", L);
    hktShader.setFloat("t", t);
    h0kTexture->bindImage(0);
    hktY->bindImage(1);
    hktX->bindImage(2);
    hktZ->bindImage(3);
    hktShader.dispatchCompute(N/16, N/16, 1);
    hktShader.stop();

    ifftShader.start();
    butterflyTexture->bindImage(0);
    ifftHelper(hktY, pong);
    ifftHelper(hktX, pong);
    ifftHelper(hktZ, pong);
    ifftShader.stop();

    combineShader.start();
    combineShader.setInt("N", N);
    hktY->bindImage(0);
    hktX->bindImage(1);
    hktZ->bindImage(2);
    displacementMap->bindImage(3);
    combineShader.dispatchCompute(N/16, N/16, 1);
    combineShader.stop();

    Engine::engine->computeNormalMap(displacementMap, normalMap, normalStrength);
}

void FFTWater::computeH0k() {
    std::vector<float> data(N * N * 4);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float n = i - N/2;
            float m = j - N/2;
            vec2 k(2.0f * M_PI * n / L, 2.0f * M_PI * m / L);
            vec2 pos = h0k(k);
            vec2 neg = h0k(-k);
            data[4*(i*N+j) + 0] = pos.r;
            data[4*(i*N+j) + 1] = pos.g;
            data[4*(i*N+j) + 2] = neg.r;
            data[4*(i*N+j) + 3] = neg.g;
        }
    }
    h0kTexture->bind(0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, N, N, 0, GL_RGBA, GL_FLOAT, data.data());
}

static unsigned maybeBitReverse(unsigned x, bool b, unsigned logN) {
    if (b) {
        unsigned ans = 0;
        for (int i = 0; i < logN; i++) {
            if ((x >> i) & 1) {
                ans |= (1 << (logN - 1 - i));
            }
        }
        return ans;
    }
    return x;
}
void FFTWater::computeButterfly() {
    std::vector<float> data(logN * N * 4);
    for (int x = 0; x < logN; x++) {
        for (int y = 0; y < N; y++) {
            int yy = y;
            float k = fmodf(float(y * N) / (1 << (x + 1)), N);
            int diff;
            if (x == 0) diff = 1;
            else diff = 1 << x;
            data[4*(yy*logN+x) + 0] = cosf(2.0f * M_PI * k / N);
            data[4*(yy*logN+x) + 1] = sinf(2.0f * M_PI * k / N);
            if (fmodf(y, 1 << (x + 1)) < (1 << x)) { // top wing
                data[4*(yy*logN+x) + 2] = maybeBitReverse(y, x == 0, logN);
                data[4*(yy*logN+x) + 3] = maybeBitReverse(y + diff, x == 0, logN);
            } else {
                data[4*(yy*logN+x) + 2] = maybeBitReverse(y - diff, x == 0, logN);
                data[4*(yy*logN+x) + 3] = maybeBitReverse(y, x == 0, logN);
            }
        }
    }
    butterflyTexture->bind(0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, logN, N, 0, GL_RGBA, GL_FLOAT, data.data());
}

vec2 FFTWater::h0k(vec2 k) {
    float L = V*V / 9.81f;
    float lenk = max(length(k), 0.001f);
    float Ph = A * (expf(-1.0f / (lenk*L*lenk*L)) / powf(lenk, 4.0f))
                 * powf(fabsf(dot(k / lenk, normalize(w))), 2.0f);
    static std::mt19937 rng(0x1337);
    static std::normal_distribution<float> nd(0.0f, 1.0f);
    float re = nd(rng);
    float im = nd(rng);
    return 1.0f / sqrtf(2.0f) * vec2(re, im) * sqrtf(Ph); 
}
