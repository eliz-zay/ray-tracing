#pragma once

#include <glm/glm.hpp>

using namespace glm;

namespace RayHelper {
    float norm(vec3 vec) {
        return std::sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
    }

    vec3 reflect(vec3 ray, vec3 normal) {
        return ray - normal * 2.f * dot(ray, normal);
    }

    vec3 refract(vec3 ray, vec3 normal, float eta_t, const float eta_i = 1.f) { // Snell's law
        float cosi = -std::max(-1.f, std::min(1.f, dot(ray, normal)));
        if (cosi < 0) {
            return refract(ray, -normal, eta_i, eta_t); // if the ray comes from the inside the object, swap the air and the media
        }

        float eta = eta_i / eta_t;
        float k = 1 - eta * eta * (1 - cosi * cosi);

        return k < 0 ? vec3(1,0,0) : ray * eta + normal * (eta * cosi - sqrtf(k));
    }
}