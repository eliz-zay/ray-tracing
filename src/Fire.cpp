#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <limits>
#include <iostream>
#include <fstream>
#include <vector>

#include "vec/vec3.cpp"
#include "vec/vec4.cpp"

#include "Fire.hpp"

const float sphere_radius   = 1.2; // all the explosion fits in a sphere with this radius. The center lies in the origin.
const float noise_amplitude = 1.0; // amount of noise applied to the sphere (towards the center)

template <typename T> inline T lerp(const T &v0, const T &v1, float t) {
    return v0 + (v1-v0)*std::max(0.f, std::min(1.f, t));
}

float myHash(const float n) {
    float x = sin(n)*43758.5453f;
    return x-floor(x);
}

float noise(const vec3 &v) {
    vec3 p(floor(v.x()), floor(v.y()), floor(v.z()));
    vec3 f(v.x()-p.x(), v.y()-p.y(), v.z()-p.z());
    f = f*(f*(vec3(3.f, 3.f, 3.f)-f*2.f));
    float n = dot(p,vec3(1.f, 57.f, 113.f));
    return lerp(lerp(
                     lerp(myHash(n +  0.f), myHash(n +  1.f), f.x()),
                     lerp(myHash(n + 57.f), myHash(n + 58.f), f.x()), f.y()),
                lerp(
                    lerp(myHash(n + 113.f), myHash(n + 114.f), f.x()),
                    lerp(myHash(n + 170.f), myHash(n + 171.f), f.x()), f.y()), f.z());
}

vec3 rotate(const vec3 &v) {
    return vec3(dot(vec3(0.00,  0.80,  0.60),v), dot(vec3(-0.80,  0.36, -0.48),v), dot(vec3(-0.60, -0.48,  0.64),v));
}

float fractal_brownian_motion(const vec3 &x) { // this is a bad noise function with lots of artifacts. TODO: find a better one
    vec3 p = rotate(x);
    float f = 0;
    f += 0.5000*noise(p); p = p*2.32;
    f += 0.2500*noise(p); p = p*3.03;
    f += 0.1250*noise(p); p = p*2.61;
    f += 0.0625*noise(p);
    return f/0.9375;
}

vec3 palette_fire(const float d) { // simple linear gradent yellow-orange-red-darkgray-gray. d is supposed to vary from 0 to 1
    const vec3   yellow(1.7, 1.3, 1.0); // note that the color is "hot", i.e. has components >1
    const vec3   orange(1.0, 0.6, 0.0);
    const vec3      red(1.0, 0.0, 0.0);
    const vec3 darkgray(0.2, 0.2, 0.2);
    const vec3     gray(0.4, 0.4, 0.4);

    float x = std::max(0.f, std::min(1.f, d));
    if (x<.25f)
        return lerp(gray, darkgray, x*4.f);
    else if (x<.5f)
        return lerp(darkgray, red, x*4.f-1.f);
    else if (x<.75f)
        return lerp(red, orange, x*4.f-2.f);
    return lerp(orange, yellow, x*4.f-3.f);
}

float signed_distance(const vec3 &p) { // this function defines the implicit surface we render
    float displacement = -fractal_brownian_motion(p*3.4)*noise_amplitude;
    return p.norm() - (sphere_radius + displacement);
}

vec3 distance_field_normal(const vec3 &pos) { // simple finite differences, very sensitive to the choice of the eps constant
    const float eps = 0.01;
    float d = signed_distance(pos);
    float nx = signed_distance(pos + vec3(eps, 0, 0)) - d;
    float ny = signed_distance(pos + vec3(0, eps, 0)) - d;
    float nz = signed_distance(pos + vec3(0, 0, eps)) - d;
    return normalize(vec3(nx, ny, nz));
}

bool sphere_trace(const vec3 &orig, const vec3 &dir, vec3 &pos) {         // Notice the early discard; in fact I know that the noise() function produces non-negative values,
    if (dot(orig,orig) - pow(dot(orig,dir), 2) > pow(sphere_radius, 2)) return false;  // thus all the explosion fits in the sphere. Thus this early discard is a conservative check.
                                                                             // It is not necessary, just a small speed-up

    pos = orig - vec3(0, 1, -12);

    for (size_t i=0; i<128; i++) {
        float d = signed_distance(pos);
        if (d < 0) {
            return true;
        }
        pos = pos + dir*std::max(d*0.1f, .01f); // note that the step depends on the current distance, if we are far from the surface, we can do big steps
    }

    return false;
}

Fire::Fire(vec3 center, float radius) {
    this->center = center;
    this->radius = radius;
}

bool Fire::intersection(vec3 origin, vec3 dir, float* distance, vec3* color, vec3* normal) {
    vec3 hit;
    if (!sphere_trace(origin, dir, hit)) {
        return false;
    }

    float noise_level = (sphere_radius-hit.norm())/noise_amplitude;
    vec3 light_dir = normalize((vec3(10, 10, 10) - hit));                     // one light is placed to (10,10,10)
    float light_intensity  = std::max(0.4f, dot(light_dir,distance_field_normal(hit)));

    *distance = (origin - hit).norm();
    *normal = normalize(hit - this->center);
    *color = palette_fire((-.2 + noise_level)*2) * light_intensity;

    return true;
}