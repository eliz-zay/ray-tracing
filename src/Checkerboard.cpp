#pragma once

#include "Checkerboard.hpp"

#include "Scene.cpp"
#include "Material.cpp"

Checkerboard::Checkerboard(
    std::pair<vec3, vec3> color, 
    float equationY, 
    float borderX, 
    float lowBorderZ, 
    float uppBorderZ
) {
    this->color = color;
    this->equationY = equationY;
    this->borderX = borderX;
    this->lowBorderZ = lowBorderZ;
    this->uppBorderZ = uppBorderZ;
}

bool Checkerboard::intersection(vec3 orig, vec3 dir, float* distance, vec3* color, vec3* normal) {
    if (fabs(dir.y()) < 1e-3)  { // only for horizontal board
        *distance = -1;
        return false;
    }

    float hitDistance = -(orig.y() - this->equationY) / dir.y();
    vec3 hit = orig + dir * hitDistance;
    if (
        hitDistance > 0 && 
        fabs(hit.x()) < this->borderX && 
        hit.z() < this->uppBorderZ && 
        hit.z() > this->lowBorderZ
    ) {
        *distance = hitDistance;
        *color = (int(.5 * hit.x() + 1000) + int(.5 * hit.z())) & 1 ? this->color.first : this->color.second;
        *normal = vec3(0, 1, 0); // only for horizontal board
    } else {
        return false;
    }

    return true;
}

void Checkerboard::setMaterial(Material* material) {
    this->material = material;
}

Material* Checkerboard::getMaterial() {
    return this->material;
}