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

        static Pyramid* pyramid;
        static std::vector<Light*> lights;
        static std::vector<Checkerboard*> checkerboards;

        Scene();

    public:
        static vec3 intersect(vec3 origin, vec3 dir);

        static void addPyramid(Pyramid* pyramid);
        static void addLight(Light* light);
        static void addBoard(Checkerboard* board);

        static vec3 getBackgroundColor();
        static Pyramid* getPyramid();
        static std::vector<Light*> getLights();
        static std::vector<Checkerboard*> getCheckerboards();
};

#endif