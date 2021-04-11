#pragma once

#include "vec/vec3.cpp"
#include "vec/vec4.cpp"

namespace RayHelper {
    vec3 reflect(vec3 ray, vec3 normal) {
        return ray -  2.f * dot(ray, normal) * normal;
    }

    vec3 refract(vec3 ray, vec3 normal, float etaTarget, float etaIn = 1.f) { // Snell's law
        float cos1 = -dot(ray, normal);
        if (cos1 < 0.f) { // ray goes from inside the pyramid
            return refract(ray, -normal, etaIn, etaTarget);
        }

        float eta = etaIn / etaTarget;
        float sin2 = eta * sqrtf(1 - cos1 * cos1);

        if (sin2 > 1.f) { // total internal reflection
            return reflect(ray, normal);
        }

        return eta * ray + normal * (eta * cos1 - sqrtf(1 - sin2 * sin2));
    }
}