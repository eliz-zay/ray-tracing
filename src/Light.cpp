#pragma once

#include <src/Light.hpp>

Light::Light(vec3 origin, float intensity) {
    this->origin = origin;
    this->intensity = intensity;
}

vec3 Light::getOrigin() {
    return this->origin;
}

float Light::getIntensity() {
    return this->intensity;
}