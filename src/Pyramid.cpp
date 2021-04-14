#pragma once

#include <limits>
#include <cmath>

#include "Pyramid.hpp"

#include "Scene.cpp"
#include "Material.cpp"

Pyramid::Pyramid(vec3 vert0, vec3 vert1, vec3 vert2, vec3 vert3, vec3 vert4, vec3 color): 
vertices(vert0, vert1, vert2, vert3, vert4) {
    for (int i = 0; i < 6; i++) {
        this->edgeColor.insert({i, color});
    }

    this->edgeCoord.insert({0, {this->vertices.upper, this->vertices.base[0], this->vertices.base[1]}});
    this->edgeCoord.insert({1, {this->vertices.upper, this->vertices.base[1], this->vertices.base[2]}});
    this->edgeCoord.insert({2, {this->vertices.upper, this->vertices.base[2], this->vertices.base[3]}});
    this->edgeCoord.insert({3, {this->vertices.upper, this->vertices.base[3], this->vertices.base[0]}});
    this->edgeCoord.insert({4, {this->vertices.base[0], this->vertices.base[2], this->vertices.base[1]}});
    this->edgeCoord.insert({5, {this->vertices.base[0], this->vertices.base[3], this->vertices.base[2]}});
}

bool Pyramid::intersection(vec3 orig, vec3 dir, float* distance, vec3* color, vec3* normal) {
    const int edgeNum = 6;
    float edgeDistance[edgeNum];

    for (int i = 0; i < edgeNum; i++) {
        std::vector<vec3> coords = this->edgeCoord[i];
        float iDist;
        if (this->triangleIntersection(orig, dir, coords[0], coords[1], coords[2], &iDist)) {
            edgeDistance[i] = iDist;
        } else {
            edgeDistance[i] = -1;
        }
    }

    float nonZeroMin = std::numeric_limits<float>::max();
    int edgeIdx = -1;
    for (int i = 0; i < edgeNum; i++) {
        if (edgeDistance[i] > 0) {
            if (edgeDistance[i] < nonZeroMin) {
                nonZeroMin = edgeDistance[i];
                edgeIdx = i;
            }
        }
    }

    if (edgeIdx == -1) {
        return false;   
    }

    vec3 hit = orig + dir * nonZeroMin;

    if (hit == vertices.upper) {
        return false;
    }
    for (vec3 vertice: vertices.base) {
        if (hit == vertice) {
            return false;
        }
    }

    *distance = nonZeroMin;
    *color = this->edgeColor[edgeIdx];
    *normal = this->getNormal(edgeIdx);

    return true;
}

bool Pyramid::triangleIntersection(vec3 orig, vec3 dir, vec3 vert1, vec3 vert2, vec3 vert3, float* distance) {
    vec3 e1 = vert2 - vert1;
    vec3 e2 = vert3 - vert1;

    vec3 pvec = cross(dir, e2);
    float det = dot(e1, pvec);

    if (det < 1e-8 && det > -1e-8) {
        return false;
    }

    float inv_det = 1 / det;
    vec3 tvec = orig - vert1;
    float u = dot(tvec, pvec) * inv_det;
    if (u < 0 || u > 1) {
        return false;
    }

    vec3 qvec = cross(tvec, e1);
    float v = dot(dir, qvec) * inv_det;
    if (v < 0 || u + v > 1) {
        return false;
    }

    *distance = dot(e2, qvec) * inv_det;
    return true;
}

vec3 Pyramid::getNormal(int edgeIdx) {
    std::vector<vec3> vertices = this->edgeCoord[edgeIdx];

    vec3 A = vertices[2] - vertices[0];
    vec3 B = vertices[1] - vertices[0];

    return normalize(cross(A, B));
}