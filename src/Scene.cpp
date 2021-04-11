#pragma once

#include "Scene.hpp"

#include "Pyramid.cpp"
#include "Light.cpp"
#include "Checkerboard.cpp"
#include "Material.cpp"
#include "RayHelper.cpp"

#define EPS 1e-4

vec3 Scene::backColor = vec3(0.556, 0.914, 1.0);
std::vector<Pyramid*> Scene::pyramids = {};
std::vector<Light*> Scene::lights = {};
std::vector<Checkerboard*> Scene::checkerboards = {};

vec3 Scene::castRay(vec3 origin, vec3 dir, int depth = 0) {
    vec3 color, normal, hit;
    Material* material;

    bool isIntersect = Scene::intersect(&origin, &dir, &hit, &normal, &color, &material);

    if (!isIntersect || depth > 4) {
        return Scene::backColor;
    }

    vec4 albedo = material->getAlbedo();

    vec3 reflectDir = normalize(RayHelper::reflect(dir, normal));
    vec3 refractDir = normalize(RayHelper::refract(dir, normal, material->getRefractIdx()));

    vec3 reflectOrig = dot(reflectDir, normal) < 0 ? hit - normal * EPS : hit + normal * EPS;
    vec3 refractOrig = dot(refractDir, normal) < 0 ? hit - normal * EPS : hit + normal * EPS;

    vec3 reflectColor = Scene::castRay(reflectOrig, reflectDir, depth + 1);
    vec3 refractColor = Scene::castRay(refractOrig, refractDir, depth + 1);

    float diffuseLightIntensity = 0, specularLightIntensity = 0;

    for (Light* light: Scene::lights) {
        vec3 lightDir = normalize(light->getOrigin() - hit);
        float lightDistance = (light->getOrigin() - hit).norm();

        vec3 shadowOrig = dot(lightDir, normal) < 0 ? hit - normal * EPS : hit + normal * EPS;
        vec3 shadowHit, shadowNorm;
        Material* tmpMaterial;
        vec3 tmpColor;
        if (
            Scene::intersect(&shadowOrig, &lightDir, &shadowHit, &shadowNorm, &tmpColor, &tmpMaterial) && 
            (shadowHit - shadowOrig).norm() < lightDistance
        ) {
            if (tmpMaterial->getAlbedo()[3] < EPS) {
                continue;
            }
        }

        diffuseLightIntensity += light->getIntensity() * std::max(0.f, dot(lightDir, normal));
        specularLightIntensity += light->getIntensity() * powf(std::max(0.f, dot(-RayHelper::reflect(-lightDir, normal), dir)), material->getSpecularExp());
    }

    return 
        color * diffuseLightIntensity * albedo[0] + 
        vec3(1, 1, 1) * specularLightIntensity * albedo[1] +
        reflectColor * albedo[2] +
        refractColor * albedo[3];
}

bool Scene::intersect(vec3* origin, vec3* dir, vec3* hit, vec3* normal, vec3* color, Material** material) {
    float pyroDist = std::numeric_limits<float>::max(); 
    float checkerDist = -1, distance;
    vec3 pyroColor, checkerColor;
    vec3 pyroNormal, checkerNormal;
    Material* pyroMaterial;
    bool checkerIntersect, pyroIntersect = false;

    for (Pyramid* pyro: Scene::pyramids) {
        vec3 iColor, iNormal;
        float iDist;
        Material* iMaterial;

        if (pyro->intersection(*origin, *dir, &iDist, &iColor, &iNormal) && iDist < pyroDist) {
            pyroIntersect = true;
            pyroDist = iDist;
            pyroColor = iColor;
            pyroNormal = iNormal;
            pyroMaterial = pyro->getMaterial();
        }
    }

    checkerIntersect = Scene::checkerboards[0]->intersection(*origin, *dir, &checkerDist, &checkerColor, &checkerNormal);

    if (!pyroIntersect && !checkerIntersect) {
        return false;
    } else if (pyroIntersect && (!checkerIntersect || (pyroDist < checkerDist))) {
        distance = pyroDist;
        *color = pyroColor;
        *normal = pyroNormal;
        *material = pyroMaterial;
    } else {
        distance = checkerDist;
        *color = checkerColor;
        *normal = checkerNormal;
        *material = Scene::checkerboards[0]->getMaterial();
    }

    *hit = (*origin) + (*dir) * distance;

    return true;
}

void Scene::addPyramid(Pyramid* pyramid) {
    Scene::pyramids.push_back(pyramid);
}

void Scene::addLight(Light* light) {
    Scene::lights.push_back(light);
}

void Scene::addBoard(Checkerboard* board) {
    Scene::checkerboards.push_back(board);
}