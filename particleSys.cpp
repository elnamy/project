#define GLM_ENABLE_EXPERIMENTAL
#include "particleSys.h"
#include "GLSL.h"
#include <algorithm>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

using namespace std;

float particleSys::randFloat(float l, float h) {
    float r = rand() / (float)RAND_MAX;
    return (1.0f - r) * l + r * h;
}

particleSys::particleSys(vec3 source, float life) {

    numP = 300;
    t = 0.0f;
    h = life; // 0.005 - 0.03
    g = vec3(0.0f, -0.098, 0.0f);
    start = source;
    theCamera = glm::mat4(1.0);
}

void particleSys::fountainGpuSetup() {

    for (int i = 0; i < numP; i++) {

        pointColors[i * 4 + 0] = randFloat(0.0, 0.3); // 0.9
        pointColors[i * 4 + 1] =
            randFloat(0.0, 1.0); // make random, one vary //0.4
        pointColors[i * 4 + 2] = randFloat(0.7, 0.9); // randFloat(0.0, 1.0)
        pointColors[i * 4 + 3] = 1.0;                 // 1.0

        vec3 normal = normalize(
            vec3(randFloat(-0.1, 0.1), randFloat(-1, 1), randFloat(-0.1, 0.1)));
        pointNormals[i * 3 + 0] = normal.x;
        pointNormals[i * 3 + 1] = normal.y;
        pointNormals[i * 3 + 2] = normal.z;

        auto particle = make_shared<Particle>(start);
        particles.push_back(particle);
        particle->load(start);
    }

    // generate the VAO
    glGenVertexArrays(1, &vertArrObj);
    glBindVertexArray(vertArrObj);

    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointColors), &pointColors[0],
                 GL_STREAM_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointNormals), &pointNormals[0],
                 GL_STREAM_DRAW);

    assert(glGetError() == GL_NO_ERROR);

    std::cout << "finish GPU setup" << endl;
}

void particleSys::gpuSetup() {

    for (int i = 0; i < numP; i++) {

        pointColors[i * 4 + 0] = randFloat(0.7, 1.0); // 0.9
        pointColors[i * 4 + 1] =
            randFloat(0.7, 1.0); // make random, one vary //0.4
        pointColors[i * 4 + 2] = randFloat(0.0, 1.0); // randFloat(0.0, 1.0)
        pointColors[i * 4 + 3] = 1.0;                 // 1.0

        vec3 normal = normalize(
            vec3(randFloat(-1, 1), randFloat(-1, 1), randFloat(-1, 1)));
        pointNormals[i * 3 + 0] = normal.x;
        pointNormals[i * 3 + 1] = normal.y;
        pointNormals[i * 3 + 2] = normal.z;

        auto particle = make_shared<Particle>(start);
        particles.push_back(particle);
        particle->load(start);
    }

    // generate the VAO
    glGenVertexArrays(1, &vertArrObj);
    glBindVertexArray(vertArrObj);

    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointColors), &pointColors[0],
                 GL_STREAM_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointNormals), &pointNormals[0],
                 GL_STREAM_DRAW);

    assert(glGetError() == GL_NO_ERROR);

    std::cout << "finish GPU setup" << endl;
}

void particleSys::reSet() {
    for (int i = 0; i < numP; i++) {
        particles[i]->load(start);
    }
}

void particleSys::drawMe(std::shared_ptr<Program> prog) {
    glBindVertexArray(vertArrObj);

    glUniform3fv(prog->getUniform("centerPos"), 1, &start[0]);
    glUniform1f(prog->getUniform("totalTime"), t);

    int c_pos = prog->getAttribute("cPos");
    GLSL::enableVertexAttribArray(c_pos);
    // std::cout << "Any Gl errors1: " << glGetError() << std::endl;
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    // std::cout << "Any Gl errors2: " << glGetError() << std::endl;
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    int n_pos = prog->getAttribute("pNormal");
    GLSL::enableVertexAttribArray(n_pos);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);
    // Draw the points !
    glDrawArraysInstanced(GL_POINTS, 0, 1, numP);

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);

    glDisableVertexAttribArray(0);
}

void particleSys::update() {

    // vec3 pos;
    // vec4 test;
    t += h;

    // Sort the particles by Z
    // temp->rotate(camRot, vec3(0, 1, 0));
    // be sure that camera matrix is updated prior to this update
    // vec3 s, t_, sk;
    // vec4 p;
    // quat r;
    // glm::decompose(theCamera, s, r, t_, sk, p);
    // sorter.C = glm::toMat4(r);
    // sort(particles.begin(), particles.end(), sorter);

    // test = particles[0]->getColor();

    // go through all the particles and update the CPU buffer
    //  for (int i = 0; i < numP; i++) {
    //       pos = particles[i]->getPosition();

    // points[i * 3 + 0] = pos.x + pointNormals[i * 3 + 0] * (0.1 - 16 * t);
    //      points[i * 3 + 1] = pos.y + pointNormals[i * 3 + 1] * (0.1 - 16 *
    //      t); points[i * 3 + 2] = pos.z + pointNormals[i * 3 + 2] * (0.1 - 16
    //      * t);
    //}

    // update the GPU data
    // glBindBuffer(GL_ARRAY_BUFFER, vertBuffObj);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(points), NULL, GL_STREAM_DRAW);
    // glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*numP*3, points);

    // glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(pointColors), NULL, GL_STREAM_DRAW);
    // glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*numP*4, pointColors);

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
}