#ifndef MATERIAL
#define MATERIAL

#include "vec/vec3.cpp"
#include "vec/vec4.cpp"

class Material {
    private:
        vec4 albedo;
        float specularExp, refractiveIdx;

    public:
        Material(vec4 albedo, float specularExp, float refractiveIdx);

        vec4 getAlbedo();
        float getSpecularExp();
        float getRefractIdx();
};

#endif