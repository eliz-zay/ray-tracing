#include <omp.h>

#include <iostream>

#include <limits>
#include <cmath>
#include <fstream>
#include <vector>

#include "vec/vec3.cpp"
#include "vec/vec4.cpp"

#include "Scene.cpp"

#include "Light.cpp"
#include "Pyramid.cpp"
#include "Checkerboard.cpp"
#include "Plane.cpp"
#include "Stand.cpp"
#include "Fire.cpp"

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
    const float eps = 4e-1;
    const int samplesPerPixel = 1;
    vector<vec3> framebuffer(width * height);

    #pragma omp parallel for
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            float x =  (2 * (j + 0.5) / (float)width - 1) * tan(fov / 2.) * width / ((float)height);
            float y = -(2 * (i + 0.5) / (float)height - 1) * tan(fov / 2.);
            vec3 dir = normalize(vec3(x, y, -1));
            bool hitGlass;
            vec3 directColor = Scene::castRay(vec3(0, 0, 0), dir, &hitGlass);

            if (!hitGlass) {
                framebuffer[i * width + j] = directColor;
                continue;
            }

            vec3 sumColor;
            int sampleCount = 0;
            for (int sample = 0; sample < samplesPerPixel; sample++) {
                float x =  ((2 * (j + 0.5) + eps * sample) / (float)width - 1) * tan(fov / 2.) * width / ((float)height);
                float y = -((2 * (i + 0.5) + eps * sample) / (float)height - 1) * tan(fov / 2.);
                vec3 dir = normalize(vec3(x, y, -1));
                vec3 sampleColor = Scene::castRay(vec3(0, 0, 0), dir, &hitGlass);
                if (!hitGlass) { // if we moved and missed the glass
                    continue;
                }
                sampleCount++;
                sumColor += sampleColor;
            }
            framebuffer[i * width + j] = sumColor / sampleCount;
        }
    }

    toFile(width, height, framebuffer);
}

int main() {
    const int width = 192;
    const int height = 192;
    const int fov = M_PI / 2.;

    Material* glass = new Material("glass", vec4(0.0, 0.5, 0.1, 0.8), 125., 1.5);
    Material* rubber = new Material("rubber", vec4(0.9,  0.1, 0.0, 0.0), 50., 1.);
    Material* plastic = new Material("plastic", vec4(0.5, 0.4, 0.0, 0.0), 50., 1.);

    Pyramid* pyramid1 = new Pyramid(
        vec3(0, 3, -12),      // vertices
        vec3(-2, -1, -10),
        vec3(-2, -1, -14),
        vec3(2, -1, -14),
        vec3(2, -1, -10),
        vec3(0, 0, 0.2)        // color
    );

    Pyramid* pyramid2 = new Pyramid(
        vec3(1, 2, -20),
        vec3(-1, 0, -18),
        vec3(-1, -1, -22),
        vec3(5, -1, -22),
        vec3(5, 0, -18),
        vec3(0.3, 0.1, 0.1)
    );

    Checkerboard* board = new Checkerboard(
        std::make_pair(vec3(1, 1, 1), vec3(1, .1, .3)), // colors
        -1.5,   // y
        10,     // x bounds
        -40,    // z1
        -5      // z2
    );

    Stand* stand = new Stand(
        vec3(-3, -1.4, -9),
        vec3(-3, -1.4, -15),
        vec3(3, -1.4, -15),
        vec3(3, -1.4, -9),
        vec3(-3, -1.1, -9),
        vec3(-3, -1.1, -15),
        vec3(3, -1.1, -15),
        vec3(3, -1.1, -9),
        vec3(0.2, 0., 0.3)
    );

    Fire* fire = new Fire(vec3(0, 0.3, -12), 1);

    vec3 floor0 = vec3(-6, -2, -8), floor1 = vec3(-6, -2, -24), floor2 = vec3(6, -2, -24), floor3 = vec3(6, -2, -8);
    vec3 ceil0 = vec3(-6, 5, -8), ceil1 = vec3(-6, 5, -24), ceil2 = vec3(6, 5, -24), ceil3 = vec3(6, 5, -8);

    Plane* wall1 = new Plane(floor0, ceil0, ceil1, floor1, vec3(0.1, 0.2, 0.3));
    Plane* wall2 = new Plane(floor1, ceil1, ceil2, floor2, vec3(0.1, 0.2, 0.3));
    Plane* wall3 = new Plane(floor2, ceil2, ceil3, floor3, vec3(0.1, 0.2, 0.3));

    pyramid1->setMaterial(glass);
    pyramid2->setMaterial(rubber);
    board->setMaterial(plastic);
    wall1->setMaterial(rubber);
    wall2->setMaterial(rubber);
    wall3->setMaterial(rubber);
    stand->setMaterial(plastic);
    fire->setMaterial(new Material());

    Light* innerLight = new Light(vec3(0, 0, -12), 3.f);
    Light* light2 = new Light(vec3(0, 3, -4), 4.f);
    Light* light3 = new Light(vec3(0, 4, -10), 1.5);

    Scene::addLight(innerLight); 
    Scene::addLight(light2);
    // Scene::addLight(light3);

    Scene::addObject(pyramid1);
    // Scene::addObject(pyramid2);
    Scene::addObject(board);
    Scene::addObject(wall1);
    Scene::addObject(wall2);
    Scene::addObject(wall3);
    Scene::addObject(stand);
    Scene::addObject(fire);

    render(width, height, fov);

    return 0;
}