#ifndef LIGHT
#define LIGHT

#include <map>
#include <vector>
#include <string>

#include <glm/glm.hpp>

using namespace glm;

class Light {
    private:
        vec3 origin;
        float intensity;

    public:
        Light(vec3 origin, float intensity);

        vec3 getOrigin();
        float getIntensity();
};

#endif