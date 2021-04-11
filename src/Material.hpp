#ifndef MATERIAL
#define MATERIAL

#include "vec/vec3.cpp"
#include "vec/vec4.cpp"

using namespace std;

class Material {
    private:
        string name;

        vec4 albedo;
        float specularExp, refractiveIdx;

    public:
        Material(string name, vec4 albedo, float specularExp, float refractiveIdx);

        string getName();
        vec4 getAlbedo();
        float getSpecularExp();
        float getRefractIdx();
};

#endif