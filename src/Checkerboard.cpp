#pragma once

#include <src/Checkerboard.hpp>

#include <src/Scene.cpp>

Checkerboard::Checkerboard(std::pair<vec3, vec3> color, float equationY, float borderX, float lowBorderZ, float uppBorderZ) {
    this->color = color;
    this->equationY = equationY;
    this->borderX = borderX;
    this->lowBorderZ = lowBorderZ;
    this->uppBorderZ = uppBorderZ;
}

void Checkerboard::castRay(vec3 orig, vec3 dir, float* checkerDist, vec3* color) {
    *checkerDist = -1;
    *color = Scene::getBackgroundColor();

    if (fabs(dir.y) > 1e-3)  {
        float hitDistance = -(orig.y - this->equationY) / dir.y;
        vec3 hit = orig + dir * hitDistance;
        if (
            hitDistance > 0 && 
            fabs(hit.x) < this->borderX && 
            hit.z < this->uppBorderZ && 
            hit.z > this->lowBorderZ
        ) {
            *checkerDist = hitDistance;
            *color = (int(.5 * hit.x + 1000) + int(.5 * hit.z)) & 1 ? this->color.first : this->color.second;
        }
    }
}