#include <iostream>

#include <limits>
#include <cmath>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>
#include "glm/ext.hpp"

#include <src/Scene.cpp>

#include <src/Pyramid.cpp>
#include <src/Light.cpp>
#include <src/Checkerboard.cpp>

using namespace std;

void toFile(int width, int height, vector<glm::vec3> framebuffer) {
    ofstream file;
    file.open("./out.ppm");
    file << "P6\n" << width << " " << height << "\n255\n";
    for (size_t i = 0; i < width * height; i++) {
        vec3  &c = framebuffer[i];
        float max = std::max(c[0], std::max(c[1], c[2]));
        if (max>1) c = c*(1./max);
        for (size_t j = 0; j < 3; j++) {
            file << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
        }
    }

    file.close();
}

void render(int width, int height, int fov) {
    vector<glm::vec3> framebuffer(width * height);

    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            float x =  (2 * (j + 0.5) / (float)width  - 1) * tan(fov / 2.) * width / (float)height;
            float y = -(2 * (i + 0.5) / (float)height - 1) * tan(fov / 2.);
            vec3 dir = normalize(vec3(x, y, -1));
            framebuffer[i * width + j] = Scene::castRay(vec3(0, 0, 0), dir);
        }
    }

    toFile(width, height, framebuffer);
}

int main() {
    const int width = 1024;
    const int height = 768;
    const int fov = M_PI / 2.;

    Light* light1 = new Light(vec3(4, 0, -8), 3.f);
    Light* light2 = new Light(vec3(0, 6, -9), 3.f);
    Light* light3 = new Light(vec3(3, 2, -16), 3.f);

    Pyramid* pyramid1 = new Pyramid(
        vec3(0, 2, -12),
        vec3(-2, -2, -10),
        vec3(-2, -3, -14),
        vec3(4, -3, -14),
        vec3(2, -2, -10),
        vec4(0.5, 0.5, 0.1, 0.8),
        125., 
        3.5
    );

    Checkerboard* board = new Checkerboard(
        std::make_pair(vec3(1, 1, 1), vec3(1, .7, .3)),
        -4,
        10,
        -30,
        -10,
        vec4(0.5, 0.4, 0.0, 0.0),
        50.,
        1.
    );

    Scene::addPyramid(pyramid1);
    // Scene::addPyramid(pyramid2);
    // Scene::addLight(light1);
    // Scene::addLight(light2);
    Scene::addLight(light3);
    Scene::addBoard(board);

    render(width, height, fov);

    return 0;
}