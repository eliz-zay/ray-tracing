#ifndef MATERIAL
#define MATERIAL

#include "vec/vec3.cpp"
#include "vec/vec4.cpp"

using namespace std;

class Material {
    private:
        string name;

        bool reflective;

        vec4 albedo;
        float specularExp, refractiveIdx;

    public:
        Material(string name, vec4 albedo, float specularExp, float refractiveIdx, bool reflective);
        Material(string name); // for non-reflective material

        string getName();
        bool isReflective();
        vec4 getAlbedo();
        float getSpecularExp();
        float getRefractIdx();
};

#endif