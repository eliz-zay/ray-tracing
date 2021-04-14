#pragma once

#include <iostream>

#include "Plane.hpp"

using namespace std;

Plane::Plane(float width, float height, vec3 point, vec3 normal, vec3 color) {
    this->width = width;
    this->height = height;
    this->point = point;
    this->normal = normal;
    this->color = color;
}

bool Plane::intersection(vec3 origin, vec3 dir, float* distance, vec3* color, vec3* normal) {
    const float EPS = 1e-6;
    vec3 diff = origin - this->point;
    float prod1 = dot(diff, this->normal);
    float prod2 = dot(dir, this->normal);

    if (fabs(prod2) < EPS) {
        return false;
    }

    float prod3 = prod1 / prod2;

    if (fabs(prod1) < EPS) {
        return false;
    }

    vec3 p = origin - dir * prod3;
    float t = (p - origin).norm();

    if (t < 0 || t > numeric_limits<float>::infinity()) {
        return false;
    }

    vec3 temp_vec;
    if (this->normal == vec3(0, 0, 1)) {
        temp_vec = vec3(0, 1, 0);
    } else {
        temp_vec = vec3(0, 0, 1);
    }

    vec3 pos_diff = this->point - p;

    vec3 width_basis = normalize(cross(this->normal, temp_vec));
    vec3 height_basis = normalize(cross(width_basis, this->normal));

    vec3 width_proj = dot(width_basis, pos_diff) * width_basis;
    vec3 height_proj = dot(height_basis, pos_diff) * height_basis;

    float width_proj_metric = fabs(width_proj.x()) > EPS ? width_proj.x() : fabs(width_proj.y()) > EPS ? width_proj.y() : width_proj.z();
    float height_proj_metric = fabs(height_proj.x()) > EPS ? height_proj.x() : fabs(height_proj.y()) > EPS ? height_proj.y() : height_proj.z();
    float width_basis_metric = fabs(width_basis.x()) > EPS ? width_basis.x() : fabs(width_basis.y()) > EPS ? width_basis.y() : width_basis.z();
    float height_basis_metric = fabs(height_basis.x()) > EPS ? height_basis.x() : fabs(height_basis.y()) > EPS ? height_basis.y() : height_basis.z();

    if (abs(width_proj_metric / width_basis_metric) > width) {
        return false;
    } 

    if (abs(height_proj_metric / height_basis_metric) > height) {
        return false;
    }

    
    vec3 hit = origin - dir * prod3;

    *distance = (hit - origin).norm();
    *color = this->color;
    *normal = this->normal;

    return true;
}

vec3 Plane::getNormal() {
    return this->normal;
}