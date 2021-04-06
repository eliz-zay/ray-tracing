#pragma once

#include <src/Scene.hpp>

#include <src/Pyramid.cpp>
#include <src/Light.cpp>
#include <src/Checkerboard.cpp>

#define EPS 1e-3

vec3 Scene::backColor = vec3(0.556, 0.914, 1.0);
Pyramid* Scene::pyramid = nullptr;
std::vector<Light*> Scene::lights = {};
std::vector<Checkerboard*> Scene::checkerboards = {};

vec3 Scene::intersect(vec3 origin, vec3 dir) {
    float pyroDist, checkerDist = -1;
    vec3 pyroColor, checkerColor;

    Scene::pyramid->castRay(origin, dir, &pyroDist, &pyroColor);
    Scene::checkerboards[0]->castRay(origin, dir, &checkerDist, &checkerColor);

    if (fabs(pyroDist + 1) < EPS && fabs(checkerDist + 1) < EPS) {
        return Scene::backColor;
    } else if (pyroDist > 0 && ((checkerDist > 0 && pyroDist < checkerDist) || (fabs(checkerDist + 1) < EPS))) {
        return pyroColor;
    } else {
        return checkerColor;
    }
}

vec3 Scene::getBackgroundColor() {
    return Scene::backColor;
}

void Scene::addPyramid(Pyramid* pyramid) {
    Scene::pyramid = pyramid;
}

void Scene::addLight(Light* light) {
    Scene::lights.push_back(light);
}

void Scene::addBoard(Checkerboard* board) {
    Scene::checkerboards.push_back(board);
}

Pyramid* Scene::getPyramid() {
    return Scene::pyramid;
}

std::vector<Light*> Scene::getLights() {
    return Scene::lights;
}

std::vector<Checkerboard*> Scene::getCheckerboards() {
    return Scene::checkerboards;
}