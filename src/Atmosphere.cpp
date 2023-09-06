#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

static vec2 rsi(vec3 r0, vec3 rd, float radius) {
    float a = dot(rd, rd);
    float b = 2.0f * dot(rd, r0);
    float c = dot(r0, r0) - (radius * radius);
    float d = (b*b) - 4.0f*a*c;
    if (d < 0.0f) return vec2(1e5, -1e5);
    return vec2(
        (-b - sqrtf(d)) / (2.0f * a),
        (-b + sqrtf(d)) / (2.0f * a)
    );
}

vec3 computeAtmosphereColour(vec3 toLightVector, vec3 ray) {
    ray = glm::normalize(ray);
    float pitch = asinf(ray.y);
    float dest = pitch;
    dest += radians(90.0f);
    dest /= radians(180.f);
    dest *= radians(170.0f);
    dest -= radians(80.0f);
    dest = max(dest, radians(5.0f));
    float diff = dest - pitch;
    mat4 mat = mat4(1.0f);
    mat = rotate(mat, diff, normalize(vec3(-ray.z, 0.0f, ray.x)));
    ray = vec3(mat * vec4(ray, 1.0f));
    const vec3 sun = normalize(toLightVector);
    const vec3 origin = vec3(0, 6372e3, 0);
    const float intensity = 48.0f;
    const float planetRadius = 6371e3;
    const float atmosphereRadius = 6471e3;
    const vec3 kR = vec3(5.5e-6, 13.0e-6, 22.4e-6);
    const float kM = 21e-6;
    const float scaleR = 8e3;
    const float scaleM = 1.2e3;
    const float g = 0.758f;
    const int primarySteps = 16;
    const int secondarySteps = 8;

    vec2 p = rsi(origin, ray, atmosphereRadius);
    if (p.x > p.y) return vec3(0.0f);
    p.y = min(p.y, rsi(origin, ray, planetRadius).x);
    float primaryStepSize = (p.y - p.x) / float(primarySteps);
    float primaryTime = 0.0f;
    float primaryAccR = 0.0f, primaryAccM = 0.0f;
    float mu = dot(ray, sun);
    float mu2 = mu * mu;
    float g2 = g * g;
    float pR = 3.0f / (16.0f * M_PI) * (1.0f + mu2);
    float pM = 3.0f / (8.0f * M_PI) * ((1.0f - g2) * (mu2 + 1.0f)) / (powf(1.0f + g2 - 2.0f * mu * g, 1.5f) * (2.0f + g2));
    vec3 rayleigh = vec3(0.0f);
    vec3 mie = vec3(0.0f);
    for (int i = 0; i < primarySteps; i++) {
        vec3 primaryPos = origin + ray * (primaryTime + primaryStepSize * 0.5f);
        float primaryHeight = length(primaryPos) - planetRadius;
        float accStepR = expf(-primaryHeight / scaleR) * primaryStepSize;
        float accStepM = expf(-primaryHeight / scaleM) * primaryStepSize;
        primaryAccR += accStepR;
        primaryAccM += accStepM;

        float secondaryStepSize = rsi(primaryPos, sun, atmosphereRadius).y / float(secondarySteps);
        float secondaryTime = 0.0f;
        float secondaryAccR = 0.0f, secondaryAccM = 0.0f;
        for (int j = 0; j < secondarySteps; j++) {
            vec3 secondaryPos = primaryPos + sun * (secondaryTime + secondaryStepSize * 0.5f);
            float secondaryHeight = length(secondaryPos) - planetRadius;
            secondaryAccR += expf(-secondaryHeight / scaleR) * secondaryStepSize;
            secondaryAccM += expf(-secondaryHeight / scaleM) * secondaryStepSize;
            secondaryTime += secondaryStepSize;
        }

        vec3 attenuation;
        for (int a = 0; a < 3; a++) {
            attenuation[a] = expf(-(kM * (primaryAccM + secondaryAccM) + kR[a] * (primaryAccR + secondaryAccR)));
        }
        rayleigh += accStepR * attenuation;
        mie += accStepM * attenuation;
        primaryTime += primaryStepSize;
    }

    vec3 colour = intensity * (pR * kR * rayleigh + pM * kM * mie);
    for (int c = 0; c < 3; c++) {
        colour[c] = 1.0f - expf(-1.0f * colour[c]);
    }
    return colour;
}
