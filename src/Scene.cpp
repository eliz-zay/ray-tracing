#pragma once

#include "Scene.hpp"

#include "Object.cpp"
#include "Pyramid.cpp"
#include "Light.cpp"
#include "Checkerboard.cpp"
#include "Plane.cpp"
#include "Material.cpp"
#include "RayHelper.cpp"

#define EPS 1e-4

vec3 Scene::backColor = vec3(0.556, 0.914, 1.0);

vector<Object*> Scene::objects = {};
vector<Light*> Scene::lights = {};

vec3 Scene::castRay(vec3 origin, vec3 dir, bool* hitGlass, int depth = 0) {
    vec3 color, normal, hit;
    Material* material;

    bool isIntersect = Scene::intersect(&origin, &dir, &hit, &normal, &color, &material);

    if (!isIntersect || depth > 4) {
        return Scene::backColor;
    }

    if (!material->isReflective()) {
        return color;
    }

    if (depth == 0) {
        *hitGlass = material->getName() == "glass" ? true : false;
    }

    vec4 albedo = material->getAlbedo();

    vec3 reflectDir = normalize(RayHelper::reflect(dir, normal));
    vec3 refractDir = normalize(RayHelper::refract(dir, normal, material->getRefractIdx()));

    vec3 reflectOrig = dot(reflectDir, normal) < 0 ? hit - normal * EPS : hit + normal * EPS;
    vec3 refractOrig = dot(refractDir, normal) < 0 ? hit - normal * EPS : hit + normal * EPS;

    vec3 reflectColor = Scene::castRay(reflectOrig, reflectDir, nullptr, depth + 1);
    vec3 refractColor = Scene::castRay(refractOrig, refractDir, nullptr, depth + 1);

    float reflectWeight = RayHelper::SchlickApproximation(dir, normal, material->getRefractIdx());

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
            (shadowHit - shadowOrig).norm() < lightDistance &&
            tmpMaterial->getAlbedo()[3] < EPS
        ) {
                continue;
        }

        diffuseLightIntensity += light->getIntensity() * std::max(0.f, dot(lightDir, normal));
        specularLightIntensity += light->getIntensity() * powf(std::max(0.f, dot(-RayHelper::reflect(-lightDir, normal), dir)), material->getSpecularExp());
    }

    return
        color * diffuseLightIntensity * albedo[0] + 
        vec3(1, 1, 1) * specularLightIntensity * albedo[1] +
        reflectColor * albedo[2] * reflectWeight +
        refractColor * albedo[3] * (1 - reflectWeight);
}

bool Scene::intersect(vec3* origin, vec3* dir, vec3* hit, vec3* normal, vec3* color, Material** material) {
    float hitDist = numeric_limits<float>::max();
    bool hitScene = false;
    vec3 hitColor, hitNormal;

    Object* hitObj;

    for (Object* obj: Scene::objects) {
        vec3 iColor, iNormal;
        float iDist;
        if (obj->intersection(*origin, *dir, &iDist, &iColor, &iNormal) && iDist < hitDist) {
            hitScene = true;
            hitDist = iDist;
            hitNormal = iNormal;
            hitColor = iColor;
            hitObj = obj;
        }
    }

    if (!hitScene) {
        return false;
    }

    *hit = (*origin) + (*dir) * hitDist;
    *normal = hitNormal;
    *color = hitColor;
    *material = hitObj->getMaterial();

    return true;
}

void Scene::addObject(Object* obj) {
    Scene::objects.push_back(obj);
}

void Scene::addLight(Light* light) {
    Scene::lights.push_back(light);
}