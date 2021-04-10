#pragma once

#include <src/Material.hpp>

Material::Material(vec4 albedo, float specularExponent, float refractiveIdx) {
    this->albedo = albedo;
    this->specularExp = specularExponent;
    this->refractiveIdx = refractiveIdx;
}

vec4 Material::getAlbedo() {
    return this->albedo;
}

float Material::getSpecularExp() {
    return this->specularExp;
}

float Material::getRefractIdx() {
    return this->refractiveIdx;
}