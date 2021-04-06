#pragma once

#include <limits>
#include <cmath>

#include <src/Pyramid.hpp>

#include <src/Scene.cpp>

Pyramid::Pyramid(vec3 vert0, vec3 vert1, vec3 vert2, vec3 vert3, vec3 vert4, vec2 albedo, float specularExponent): 
vertices(vert0, vert1, vert2, vert3, vert4) {
    this->edgeColor.insert({0, vec3(0.0, 0.2, 0.8)});
    this->edgeColor.insert({1, vec3(0.6, 0.0, 0.4)});
    this->edgeColor.insert({2, vec3(0.0, 0.2, 0.8)});
    this->edgeColor.insert({3, vec3(0.6, 0.0, 0.4)});
    this->edgeColor.insert({4, vec3(0.3, 0.0, 0.1)});
    this->edgeColor.insert({5, vec3(0.3, 0.0, 0.1)});

    this->edgeCoord.insert({0, {this->vertices.upper, this->vertices.base[0], this->vertices.base[1]}});
    this->edgeCoord.insert({1, {this->vertices.upper, this->vertices.base[1], this->vertices.base[2]}});
    this->edgeCoord.insert({2, {this->vertices.upper, this->vertices.base[2], this->vertices.base[3]}});
    this->edgeCoord.insert({3, {this->vertices.upper, this->vertices.base[3], this->vertices.base[0]}});
    this->edgeCoord.insert({4, {this->vertices.base[0], this->vertices.base[2], this->vertices.base[1]}});
    this->edgeCoord.insert({5, {this->vertices.base[0], this->vertices.base[3], this->vertices.base[2]}});

    this->albedo = albedo;
    this->specularExponent = specularExponent;
}

vec3 Pyramid::reflect(vec3 ray, vec3 normal) {
    return ray - normal * 2.f * (ray * normal);
}

vec3 Pyramid::getNormal(int edgeIdx) {
    std::vector<vec3> vertices = this->edgeCoord[edgeIdx];

    vec3 A = vertices[2] - vertices[0];
    vec3 B = vertices[1] - vertices[0];

    return normalize(cross(A, B));
}

float Pyramid::triangleIntersection(vec3 orig, vec3 dir, vec3 vert1, vec3 vert2, vec3 vert3) {
    vec3 e1 = vert2 - vert1;
    vec3 e2 = vert3 - vert1;

    vec3 pvec = cross(dir, e2);
    float det = dot(e1, pvec);

    if (det < 1e-8 && det > -1e-8) {
        return 0;
    }

    float inv_det = 1 / det;
    vec3 tvec = orig - vert1;
    float u = dot(tvec, pvec) * inv_det;
    if (u < 0 || u > 1) {
        return 0;
    }

    vec3 qvec = cross(tvec, e1);
    float v = dot(dir, qvec) * inv_det;
    if (v < 0 || u + v > 1) {
        return 0;
    }
    return dot(e2, qvec) * inv_det;
}

void Pyramid::pyroIntersection(vec3 orig, vec3 dir, float* distance, int* edgeIdx) {
    const int edgeNum = 6;
    float edgeDistance[edgeNum];
    
    for (int i = 0; i < edgeNum; i++) {
        std::vector<vec3> coords = this->edgeCoord[i];
        edgeDistance[i] = this->triangleIntersection(orig, dir, coords[0], coords[1], coords[2]);
    }

    float nonZeroMin = std::numeric_limits<float>::max();
    int idx = -1;
    for (int i = 0; i < edgeNum; i++) {
        if (edgeDistance[i] && edgeDistance[i] < nonZeroMin) {
            nonZeroMin = edgeDistance[i];
            idx = i;
        }
    }

    *distance = nonZeroMin;
    *edgeIdx = idx;
}

void Pyramid::castRay(vec3 origin, vec3 dir, float* distance, vec3* color) {
    float diffuseLightIntensity = 0, specularLightIntensity = 0;
    int edgeIdx = -1;

    this->pyroIntersection(origin, dir, distance, &edgeIdx);

    if (edgeIdx == -1) {
        *color = Scene::getBackgroundColor();
        *distance = -1;
        return;
    }

    vec3 hit = origin + dir * (*distance);
    vec3 normal = this->getNormal(edgeIdx);

    for (Light* light: Scene::getLights()) {
        vec3 lightDir = normalize(light->getOrigin() - hit);
        diffuseLightIntensity += light->getIntensity() * std::max(0.f, dot(lightDir, normal));
        specularLightIntensity += powf(std::max(0.f, dot(-reflect(-lightDir, normal), dir)), this->specularExponent) * light->getIntensity();
    }

    *color = this->edgeColor[edgeIdx] * diffuseLightIntensity * this->albedo[0] + vec3(1, 1, 1) * specularLightIntensity * this->albedo[1];
}