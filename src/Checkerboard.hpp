#ifndef CHECKERBOARD
#define CHECKERBOARD

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include <src/Material.hpp>

using namespace glm;

class Checkerboard {
    private:
        std::pair<vec3, vec3> color;
        float equationY, borderX, lowBorderZ, uppBorderZ;

        Material* material;
    
    public:
        Checkerboard(
            std::pair<vec3, vec3> color, 
            float equationY,    // everything only for horizontal board (y = equationY)
            float borderX, 
            float lowBorderZ, 
            float uppBorderZ, 
            vec4 albedo, 
            float specularExponent,
            float refractiveIdx
        );

        bool intersection(vec3 orig, vec3 dir, float* distance, vec3* color, vec3* normal);
        
        Material* getMaterial();
};

#endif