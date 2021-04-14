#include <omp.h>

#include <iostream>

#include <limits>
#include <cmath>
#include <fstream>
#include <vector>

#include "vec/vec3.cpp"
#include "vec/vec4.cpp"

#include "Scene.cpp"

#include "Pyramid.cpp"
#include "Light.cpp"
#include "Checkerboard.cpp"
#include "Plane.cpp"

using namespace std;

void toFile(int width, int height, vector<vec3> framebuffer) {
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
    vector<vec3> framebuffer(width * height);

    #pragma omp parallel for
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

    Material* glass = new Material("glass", vec4(0.0, 0.5, 0.1, 0.8), 125., 1.5);
    Material* rubber = new Material("rubber", vec4(0.9,  0.1, 0.0, 0.0), 50., 1.);
    Material* plastic = new Material("plastic", vec4(0.5, 0.4, 0.0, 0.0), 50., 1.);

    Light* light1 = new Light(vec3(0, 0, -12), 3.f);
    Light* light2 = new Light(vec3(0, 6, -9), 3.f);
    Light* light3 = new Light(vec3(3, 2, -16), 3.f);

    Pyramid* pyramid1 = new Pyramid(
        vec3(0, 3, -12),            // vertices
        vec3(-2, -1, -10),
        vec3(-2, -1, -14),
        vec3(2, -1, -14),
        vec3(2, -1, -10),
        vec3(1, 1, 1)        // color
    );

    Pyramid* pyramid2 = new Pyramid(
        vec3(1, 2, -20),            // vertices
        vec3(-1, 0, -18),
        vec3(-1, -1, -22),
        vec3(5, -1, -22),
        vec3(5, 0, -18),
        vec3(0.3, 0.1, 0.1)        // color
    );

    Checkerboard* board = new Checkerboard(
        std::make_pair(vec3(1, 1, 1), vec3(1, .7, .3)), //colors
        -2, // y
        10, // x bounds
        -40,    //z1
        -10    //z2
    );

    Plane* plane = new Plane(
        10,
        10,
        vec3(7, 0, -20),
        vec3(1, 0, 0),
        vec3(0.3, 0.1, 0.1)
    );

    pyramid1->setMaterial(glass);
    pyramid2->setMaterial(rubber);
    board->setMaterial(plastic);
    plane->setMaterial(rubber);

    // Scene::addObject(pyramid1);
    // Scene::addObject(pyramid2);
    Scene::addLight(light1);
    Scene::addLight(light2);
    Scene::addLight(light3);
    Scene::addObject(board);
    Scene::addObject(plane); 

    render(width, height, fov);

    return 0;
}