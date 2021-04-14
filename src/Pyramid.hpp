#ifndef PYRAMID
#define PYRAMID

#include <map>
#include <vector>
#include <string>

#include "vec/vec3.cpp"
#include "vec/vec4.cpp"

#include "Object.hpp"
#include "Material.hpp"

using namespace std;

struct Vertices {
    vec3 upper;
    std::vector<vec3> base;

    Vertices(vec3 vert0, vec3 vert1, vec3 vert2, vec3 vert3, vec3 vert4) {
        this->upper = vert0;
        this->base = {vert1, vert2, vert3, vert4};
    }
};

class Pyramid: public Object {
    private:
        Vertices vertices;
        std::map<int, vec3> edgeColor;
        std::map<int, std::vector<vec3> > edgeCoord;

        bool triangleIntersection(vec3 origin, vec3 dir, vec3 vert1, vec3 vert2, vec3 vert3, float* distance);

    public:
        Pyramid(vec3 vert0, vec3 vert1, vec3 vert2, vec3 vert3, vec3 vert4, vec3 color);

        bool intersection(vec3 origin, vec3 dir, float* distance, vec3* color, vec3* normal);

        vec3 getNormal(int edgeIdx);
};

#endif