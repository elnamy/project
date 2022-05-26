//
// sueda - geometry edits Z. Wood
// 3/16
//

#include "Particle.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"
#include <iostream>

float randFloat(float l, float h) {
    float r = rand() / (float)RAND_MAX;
    return (1.0f - r) * l + r * h;
}

Particle::Particle(vec3 start)
    : charge(1.0f), m(1.0f), d(0.0f), x(start), v(0.0f, 0.0f, 0.0f),
      a(0.0f, -0.098f, 0.18f), // 0.13 z for coming forward -0.8 for gas cloud
      lifespan(15.0f), tEnd(0.0f), // Changed from 0.0f to 10.0f
      scale(1.0f), color(1.0f, 1.0f, 1.0f, 1.0f) {}

Particle::~Particle() {}

void Particle::load(vec3 start) {
    // Random initialization
    rebirth(0.0f, start);
}

/* all particles born at the origin */
void Particle::rebirth(float t, vec3 start) {
    charge = randFloat(0.0f, 1.0f) < 0.5 ? -1.0f : 1.0f;
    m = 1.0f;
    d = randFloat(0.0f, 0.02f);
    x = start;
    v.x = randFloat(-0.27f, 0.3f);
    v.y = randFloat(-0.1f, 0.9f);
    v.z = randFloat(-0.3f, 0.27f);
    lifespan = randFloat(3.0f, 18.0f);
    tEnd = t + lifespan;
    scale = randFloat(0.2, 1.0f);
    color.r = randFloat(0.9f, 0.99f);
    color.g = randFloat(0.9f, 0.99f);
    color.b = randFloat(0.3f, 0.8f);
    color.a = 0.5f;
}

void Particle::update(float t, float h, const vec3 &g, const vec3 start) {
    // if(t > tEnd) {
    //	rebirth(t, start);
    // }

    // very simple update
    /*std::cout << t << std::endl;*/
    v = v + a * h;
    x.x = x.x + v.x * h + (0.5) * (a.x * pow(h, 2));
    x.y = x.y + v.y * h + (0.5) * (a.y * pow(h, 2));
    x.z = x.z + v.z * h + (0.5) * (a.z * pow(h, 2));
    /*x += h*v;*/
    // To do - how do you want to update the forces?
    color.a = (tEnd - t) / lifespan;
}

void Particle::setColor(glm::vec4 newCol) { color = newCol; }