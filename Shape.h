

#pragma once

#ifndef LAB471_SHAPE_H_INCLUDED
#define LAB471_SHAPE_H_INCLUDED

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>
#include <vector>

class Program;

class Shape {

  public:
    // stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp)
    void loadMesh(const std::string &meshName, std::string *mtlName = NULL,
                  unsigned char *(loadimage)(char const *, int *, int *, int *,
                                             int) = NULL);
    void init(std::vector<glm::mat4> instancePositions);
    void resize();
    void draw(const std::shared_ptr<Program> prog) const;
    bool didCollideWith(std::shared_ptr<Shape> other);
    void applyTransformationToCollisionBuffer(glm::mat4 transformation,
                                              bool shouldPrint);

    unsigned int *textureIDs = NULL;

  private:
    int obj_count = 0;
    std::vector<unsigned int> *eleBuf = NULL;
    std::vector<float> *posBuf = NULL;
    std::vector<float> *collisionBuf = NULL;
    std::vector<float> *norBuf = NULL;
    std::vector<float> *texBuf = NULL;
    std::vector<glm::mat4> *instancedBuf = NULL;
    unsigned int *materialIDs = NULL;

    unsigned int *eleBufID = 0;
    unsigned int *posBufID = 0;
    unsigned int *collisionBufID = 0;
    unsigned int *norBufID = 0;
    unsigned int *texBufID = 0;
    unsigned int *instancedBufID = 0;
    unsigned int *vaoID = 0;

    glm::vec3 min = glm::vec3(0.f);
    glm::vec3 max = glm::vec3(0.f);

    void measure();
};

#endif // LAB471_SHAPE_H_INCLUDED
