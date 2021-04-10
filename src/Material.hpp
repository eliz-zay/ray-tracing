#ifndef MATERIAL
#define MATERIAL

#include <glm/glm.hpp>

using namespace glm;

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