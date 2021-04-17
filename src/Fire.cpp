#include <cmath>
#include <algorithm>
#include <limits>
#include <iostream>

#include "vec/vec3.cpp"
#include "vec/vec4.cpp"

#include "Fire.hpp"

#define NOISE_AMPLITUDE 1.f

Fire::Fire(vec3 center, float radius) {
    this->center = center;
    this->radius = radius;
}

bool Fire::intersection(vec3 origin, vec3 dir, float* distance, vec3* color, vec3* normal) {
    vec3 hit;

    // Calculate hit in new coordinates so center = vec3(0,0,0)
    if (!sphereTrace(origin - this->center, dir, hit)) { 
        return false;
    }

    float noiseLevel = (this->radius - hit.norm()) / NOISE_AMPLITUDE;
    *color = paletteFire((-.2 + noiseLevel) * 2);

    // Move back to our coordinates
    hit = hit + this->center;
    *distance = (origin - hit).norm();
    *normal = normalize(hit - this->center);

    return true;
}

// move on until f<0 (it means we hit the sphere)
bool Fire::sphereTrace(const vec3 &orig, const vec3 &dir, vec3 &hit) {
    hit = orig; // move on from current pos

    for (size_t i = 0; i < 120; i++) {
        float d = signedDistance(hit);
        if (d < 0) {
            return true;
        }
        hit += dir * max(d*0.1f, .1f);
    }

    return false;
}

// Defines the implicit surface we render
float Fire::signedDistance(const vec3 &pos) {
    float displacement = -brownianMotion(pos * 3.4) * NOISE_AMPLITUDE;
    return pos.norm() - (this->radius + displacement);
}

// Get normal
vec3 Fire::distanceFieldNormal(const vec3 &pos) { // simple finite differences, very sensitive to the choice of the eps constant
    const float eps = 0.01;
    float d = signedDistance(pos);
    float nx = signedDistance(pos + vec3(eps, 0, 0)) - d;
    float ny = signedDistance(pos + vec3(0, eps, 0)) - d;
    float nz = signedDistance(pos + vec3(0, 0, eps)) - d;
    return normalize(vec3(nx, ny, nz));
}

// Noise function
float Fire::brownianMotion(const vec3 &x) {
    // rotate
    vec3 p = vec3(dot(vec3(0.00,  0.80,  0.60), x), dot(vec3(-0.80,  0.36, -0.48), x), dot(vec3(-0.60, -0.48,  0.64), x));
    float f = 0;
    f += 0.5000 * noise(p); p = p * 2.32;
    f += 0.2500 * noise(p); p = p * 3.03;
    f += 0.1250 * noise(p); p = p * 2.61;
    f += 0.0625 * noise(p);
    return f/0.9375;
    // f = noise(x);
    return f;
}

float Fire::noise(const vec3 &v) {
    vec3 p(floor(v.x()), floor(v.y()), floor(v.z()));
    vec3 f = v - p;
    f = f * (f * (vec3(3.f, 3.f, 3.f) - f * 2.f));
    float n = dot(p, vec3(1.f, 57.f, 113.f));
    return lerp(lerp(
                     lerp(sinHash(n +  0.f), sinHash(n +  1.f), f.x()),
                     lerp(sinHash(n + 57.f), sinHash(n + 58.f), f.x()), f.y()),
                lerp(
                    lerp(sinHash(n + 113.f), sinHash(n + 114.f), f.x()),
                    lerp(sinHash(n + 170.f), sinHash(n + 171.f), f.x()), f.y()), f.z());
}

// Linear gradent yellow-orange-red-darkgray-gray. d is supposed to vary from 0 to 1
vec3 Fire::paletteFire(const float d) {
    const vec3    white(1.0, 1.0, 1.0);
    const vec3   yellow(1.7, 1.3, 1.0); // note that the color is "hot", i.e. has components >1
    const vec3   orange(1.0, 0.6, 0.0);
    const vec3      red(1.0, 0.0, 0.0);
    const vec3  darkred(0.7, 0.0, 0.0);
    const vec3  deepred(0.3, 0.0, 0.0);

    float x = max(0.f, min(1.f, d));
    if (x < .25f) {
        return lerp(deepred, darkred, x*4.f);
    } else if (x < .5f) {
        return lerp(darkred, red, x*4.f-1.f);
    } else if (x < .75f) {
        return lerp(red, orange, x*4.f-2.f);
    } else if (x < .95f) {
        return lerp(orange, yellow, x*4.f-3.f);
    }
    return lerp(yellow, white, x*4.f-3.f);
}

template <typename T> 
inline T Fire::lerp(const T &v0, const T &v1, float t) {
    return v0 + (v1 - v0) * max(0.f, min(1.f, t));
}

float Fire::sinHash(const float n) {
    float x = sin(n) * ((int)n >> 1) * ((int)n << 2);
    return x - floor(x);
}