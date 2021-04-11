#pragma once

#include "Material.hpp"

Material::Material(string name, vec4 albedo, float specularExponent, float refractiveIdx) {
    this->name = name;
    this->albedo = albedo;
    this->specularExp = specularExponent;
    this->refractiveIdx = refractiveIdx;
}

string Material::getName() {
    return this->name;
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