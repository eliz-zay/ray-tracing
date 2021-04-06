#ifndef CHECKERBOARD
#define CHECKERBOARD

#include <vector>
#include <string>

#include <glm/glm.hpp>

using namespace glm;

class Checkerboard {
    private:
        std::pair<vec3, vec3> color;
        float equationY, borderX, lowBorderZ, uppBorderZ;
    
    public:
        Checkerboard(std::pair<vec3, vec3> color, float equationY, float borderX, float lowBorderZ, float uppBorderZ);

        void castRay(vec3 orig, vec3 dir, float* distance, vec3* color);
};

#endif