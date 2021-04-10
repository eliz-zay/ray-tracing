#ifndef SCENE
#define SCENE

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include <src/Pyramid.hpp>
#include <src/Light.hpp>
#include <src/Checkerboard.hpp>

class Pyramid;
class Light;
class Checkerboard;

using namespace glm;

class Scene {
    private:
        static vec3 backColor;

        static std::vector<Pyramid*> pyramids;
        static std::vector<Light*> lights;
        static std::vector<Checkerboard*> checkerboards;

        Scene();

    public:
        static vec3 castRay(vec3 origin, vec3 dir, int depth);
        static bool intersect(vec3* origin, vec3* dir, vec3* hit, vec3* normal, vec3* color, Material** material);

        static void addPyramid(Pyramid* pyramid);
        static void addLight(Light* light);
        static void addBoard(Checkerboard* board);
};

#endif