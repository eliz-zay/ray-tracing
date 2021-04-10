#pragma once

#include <src/Scene.hpp>

#include <src/Pyramid.cpp>
#include <src/Light.cpp>
#include <src/Checkerboard.cpp>
#include <src/Material.cpp>
#include <src/RayHelper.cpp>

#define EPS 1e-3

vec3 Scene::backColor = vec3(0.556, 0.914, 1.0);
Pyramid* Scene::pyramid = nullptr;
std::vector<Light*> Scene::lights = {};
std::vector<Checkerboard*> Scene::checkerboards = {};

vec3 Scene::castRay(vec3 origin, vec3 dir, int depth = 0) {
    vec3 color, normal, hit;
    Material* material;

    bool isIntersect = Scene::intersect(&origin, &dir, &hit, &normal, &color, &material);

    if (!isIntersect or depth > 4) {
        return Scene::backColor;
    }

    vec3 reflectDir = normalize(RayHelper::reflect(dir, normal));
    vec3 refractDir = normalize(refract(dir, normal, material->getRefractIdx()));

    vec3 reflectOrig = dot(reflectDir, normal) < 0 ? hit - normal * EPS : hit + normal * EPS; // offset the original point to avoid occlusion by the object itself
    vec3 refractOrig = dot(refractDir, normal) < 0 ? hit - normal * EPS : hit + normal * EPS;

    vec3 reflectColor = Scene::castRay(reflectOrig, reflectDir, depth + 1);
    vec3 refractColor = Scene::castRay(refractOrig, refractDir, depth + 1);

    float diffuseLightIntensity = 0, specularLightIntensity = 0;

    for (Light* light: Scene::getLights()) {
        vec3 lightDir = normalize(light->getOrigin() - hit);
        float lightDistance = RayHelper::norm(light->getOrigin() - hit);

        vec3 shadowOrig = dot(lightDir, normal) < 0 ? hit - normal * EPS : hit + normal * EPS;
        vec3 shadowHit, shadowNorm;
        Material* tmpMaterial;
        vec3 tmpColor;
        if (Scene::intersect(&shadowOrig, &lightDir, &shadowHit, &shadowNorm, &tmpColor, &tmpMaterial) && RayHelper::norm(shadowHit - shadowOrig) < lightDistance)
            continue;

        diffuseLightIntensity += light->getIntensity() * std::max(0.f, dot(lightDir, normal));
        specularLightIntensity += powf(std::max(0.f, dot(-RayHelper::reflect(-lightDir, normal), dir)), material->getSpecularExp()) * light->getIntensity();
    }

    vec4 albedo = material->getAlbedo();

    return 
        color * diffuseLightIntensity * albedo[0] + 
        vec3(1, 1, 1) * specularLightIntensity * albedo[1] +
        reflectColor * albedo[2] +
        refractColor * albedo[3];
}

bool Scene::intersect(vec3* origin, vec3* dir, vec3* hit, vec3* normal, vec3* color, Material** material) {
    float pyroDist = -1, checkerDist = -1, distance;
    vec3 pyroColor, checkerColor;
    vec3 pyroNormal, checkerNormal;

    Scene::pyramid->intersection(*origin, *dir, &pyroDist, &pyroColor, &pyroNormal);
    Scene::checkerboards[0]->intersection(*origin, *dir, &checkerDist, &checkerColor, &checkerNormal);

    if (fabs(pyroDist + 1) < EPS && fabs(checkerDist + 1) < EPS) {
        return false;
    } else if (pyroDist > 0 && ((checkerDist > 0 && pyroDist < checkerDist) || (fabs(checkerDist + 1) < EPS))) {
        distance = pyroDist;
        *color = pyroColor;
        *normal = pyroNormal;
        *material = Scene::pyramid->getMaterial();
    } else {
        distance = checkerDist;
        *color = checkerColor;
        *normal = checkerNormal;
        *material = Scene::checkerboards[0]->getMaterial();
    }

    *hit = (*origin) + (*dir) * distance;

    return true;
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

std::vector<Light*> Scene::getLights() {
    return Scene::lights;
}

std::vector<Checkerboard*> Scene::getCheckerboards() {
    return Scene::checkerboards;
}