#include "Shape.h"

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "GLSL.h"
#include "Program.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "Macros.h"
#include "tiny_obj_loader.h"

using namespace std;

void Shape::loadMesh(const string &meshName, string *mtlpath,
                     unsigned char *(loadimage)(char const *, int *, int *,
                                                int *, int)) {
    // Load geometry
    // Some obj files contain material information.
    // We'll ignore them for this assignment.
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> objMaterials;
    string errStr;
    bool rc = false;
    if (mtlpath)
        rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str(),
                              mtlpath->c_str());
    else
        rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str());

    if (!rc) {
        cerr << errStr << endl;
    } else if (shapes.size()) {
        obj_count = shapes.size();
        posBuf = new std::vector<float>[shapes.size()];
        collisionBuf = new std::vector<float>[shapes.size()];
        norBuf = new std::vector<float>[shapes.size()];
        texBuf = new std::vector<float>[shapes.size()];
        eleBuf = new std::vector<unsigned int>[shapes.size()];
        instancedBuf = new std::vector<glm::mat4>[shapes.size()];

        eleBufID = new unsigned int[shapes.size()];
        posBufID = new unsigned int[shapes.size()];
        collisionBufID = new unsigned int[shapes.size()];
        norBufID = new unsigned int[shapes.size()];
        texBufID = new unsigned int[shapes.size()];
        vaoID = new unsigned int[shapes.size()];
        instancedBufID = new unsigned int[shapes.size()];
        materialIDs = new unsigned int[shapes.size()];
        textureIDs = new unsigned int[shapes.size()];

        for (int i = 0; i < obj_count; i++) {
            // load textures
            textureIDs[i] = 0;
            // texture sky
            posBuf[i] = shapes[i].mesh.positions;
            collisionBuf[i] = shapes[i].mesh.positions;
            norBuf[i] = shapes[i].mesh.normals;
            texBuf[i] = shapes[i].mesh.texcoords;
            eleBuf[i] = shapes[i].mesh.indices;
            if (shapes[i].mesh.material_ids.size() > 0)
                materialIDs[i] = shapes[i].mesh.material_ids[0];
            else
                materialIDs[i] = -1;
        }
    }
}

void Shape::applyTransformationToCollisionBuffer(glm::mat4 transformation,
                                                 bool shouldPrint) {
    for (int i = 0; i < obj_count; i++) {
        for (size_t v = 0; v < posBuf[i].size() / 3; v++) {
            glm::vec4 startingPos =
                glm::vec4(posBuf[i][3 * v + 0], posBuf[i][3 * v + 1],
                          posBuf[i][3 * v + 2], 1);

            glm::vec4 newPos = transformation * startingPos;

            collisionBuf[i][3 * v + 0] = newPos.x;
            collisionBuf[i][3 * v + 1] = newPos.y;
            collisionBuf[i][3 * v + 2] = newPos.z;

            if (i == 0 && shouldPrint && DEBUG_MODE)
                cout << "current pos: " << newPos.x << " " << newPos.y << " "
                     << newPos.z << endl;
        }
    }

    measure();
}

void Shape::measure() {
    float minX, minY, minZ;
    float maxX, maxY, maxZ;

    minX = minY = minZ = std::numeric_limits<float>::max();
    maxX = maxY = maxZ = -std::numeric_limits<float>::max();

    // Go through all vertices to determine min and max of each dimension
    for (int i = 0; i < obj_count; i++) {
        // NOTE: trying to get position after transform in vertex shader?
        //        glm::vec3 newPosFromBuffer = glm::vec3(0, 0, 0);
        //        glGetBufferSubData(GL_ARRAY_BUFFER, 0, posBuf[i].size() *
        //        sizeof(glm::vec3), &newPosFromBuffer);

        for (size_t v = 0; v < collisionBuf[i].size() / 3; v++) {
            if (collisionBuf[i][3 * v + 0] < minX)
                minX = collisionBuf[i][3 * v + 0];
            if (collisionBuf[i][3 * v + 0] > maxX)
                maxX = collisionBuf[i][3 * v + 0];

            if (collisionBuf[i][3 * v + 1] < minY)
                minY = collisionBuf[i][3 * v + 1];
            if (collisionBuf[i][3 * v + 1] > maxY)
                maxY = collisionBuf[i][3 * v + 1];

            if (collisionBuf[i][3 * v + 2] < minZ)
                minZ = collisionBuf[i][3 * v + 2];
            if (collisionBuf[i][3 * v + 2] > maxZ)
                maxZ = collisionBuf[i][3 * v + 2];
        }
    }

    min.x = minX;
    min.y = minY;
    min.z = minZ;

    max.x = maxX;
    max.y = maxY;
    max.z = maxZ;
}

void Shape::resize() {
    float scaleX, scaleY, scaleZ;
    float shiftX, shiftY, shiftZ;
    float epsilon = 0.001f;

    measure();

    // From min and max compute necessary scale and shift for each dimension
    float maxExtent, xExtent, yExtent, zExtent;
    xExtent = max.x - min.x;
    yExtent = max.y - min.y;
    zExtent = max.z - min.z;

    if (xExtent >= yExtent && xExtent >= zExtent) {
        maxExtent = xExtent;
    }

    if (yExtent >= xExtent && yExtent >= zExtent) {
        maxExtent = yExtent;
    }

    if (zExtent >= xExtent && zExtent >= yExtent) {
        maxExtent = zExtent;
    }

    scaleX = 2.0f / maxExtent;
    shiftX = min.x + (xExtent / 2.0f);
    scaleY = 2.0f / maxExtent;
    shiftY = min.y + (yExtent / 2.0f);
    scaleZ = 2.0f / maxExtent;
    shiftZ = min.z + (zExtent / 2.0f);

    // Go through all verticies shift and scale them
    for (int i = 0; i < obj_count; i++)
        for (size_t v = 0; v < posBuf[i].size() / 3; v++) {
            posBuf[i][3 * v + 0] = (posBuf[i][3 * v + 0] - shiftX) * scaleX;
            assert(posBuf[i][3 * v + 0] >= -1.0f - epsilon);
            assert(posBuf[i][3 * v + 0] <= 1.0f + epsilon);
            posBuf[i][3 * v + 1] = (posBuf[i][3 * v + 1] - shiftY) * scaleY;
            assert(posBuf[i][3 * v + 1] >= -1.0f - epsilon);
            assert(posBuf[i][3 * v + 1] <= 1.0f + epsilon);
            posBuf[i][3 * v + 2] = (posBuf[i][3 * v + 2] - shiftZ) * scaleZ;
            assert(posBuf[i][3 * v + 2] >= -1.0f - epsilon);
            assert(posBuf[i][3 * v + 2] <= 1.0f + epsilon);
        }
}

bool Shape::didCollideWith(std::shared_ptr<Shape> other) {
    // collision x-axis?
    bool collisionX = this->max.x > other->min.x && this->min.x < other->max.x;

    if (collisionX && DEBUG_MODE)
        cout << "collision X: this: " << this->max.x - this->min.x
             << " other: " << other->max.x - other->min.x << endl;

    // collision y-axis?
    bool collisionY = this->max.y > other->min.y && this->min.y < other->max.y;

    if (collisionY && DEBUG_MODE)
        cout << "collision Y: this: " << this->max.y - this->min.y
             << " other: " << other->max.y - other->min.y << endl;

    // collision z-axis?
    bool collisionZ = this->max.z > other->min.z && this->min.z < other->max.z;

    if (collisionZ && DEBUG_MODE)
        cout << "collision Z: this: " << this->max.z - this->min.z
             << " other: " << other->max.z - other->min.z << endl;

    // collision only if on all axes
    return collisionX && collisionY && collisionZ;
}

void Shape::init(std::vector<glm::mat4> instancePositions) {
    for (int i = 0; i < obj_count; i++)

    {
        // Initialize the vertex array object
        glGenVertexArrays(1, &vaoID[i]);
        glBindVertexArray(vaoID[i]);

        // Send the position array to the GPU
        glGenBuffers(1, &posBufID[i]);
        glBindBuffer(GL_ARRAY_BUFFER, posBufID[i]);
        glBufferData(GL_ARRAY_BUFFER, posBuf[i].size() * sizeof(float),
                     posBuf[i].data(), GL_STATIC_DRAW);

        // Send the normal array to the GPU
        if (norBuf[i].empty()) {
            norBufID[i] = 0;
        } else {
            glGenBuffers(1, &norBufID[i]);
            glBindBuffer(GL_ARRAY_BUFFER, norBufID[i]);
            glBufferData(GL_ARRAY_BUFFER, norBuf[i].size() * sizeof(float),
                         norBuf[i].data(), GL_STATIC_DRAW);
        }

        // Send the texture array to the GPU
        if (texBuf[i].empty()) {
            texBufID[i] = 0;
        } else {
            glGenBuffers(1, &texBufID[i]);
            glBindBuffer(GL_ARRAY_BUFFER, texBufID[i]);
            glBufferData(GL_ARRAY_BUFFER, texBuf[i].size() * sizeof(float),
                         texBuf[i].data(), GL_STATIC_DRAW);
        }

        // Send the element array to the GPU
        glGenBuffers(1, &eleBufID[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     eleBuf[i].size() * sizeof(unsigned int), eleBuf[i].data(),
                     GL_STATIC_DRAW);

        if (instancePositions.empty()) {
            instancePositions.push_back(glm::mat4(1));
        }

        // Setup instanced rendering
        glGenBuffers(1, &instancedBufID[i]);
        glBindBuffer(GL_ARRAY_BUFFER, instancedBufID[i]);
        instancedBuf[i] = instancePositions;
        glBufferData(GL_ARRAY_BUFFER,
                     instancePositions.size() * sizeof(glm::mat4),
                     instancedBuf[i].data(), GL_STATIC_DRAW);

        // Set up the vertex attribute for instance rendering
        glEnableVertexAttribArray(INSTANCE_POS_ATTR);
        glVertexAttribPointer(INSTANCE_POS_ATTR, 4, GL_FLOAT, GL_FALSE,
                              4 * sizeof(glm::vec4), (void *)0);

        glEnableVertexAttribArray(INSTANCE_POS_ATTR + 1);
        glVertexAttribPointer(INSTANCE_POS_ATTR + 1, 4, GL_FLOAT, GL_FALSE,
                              4 * sizeof(glm::vec4),
                              (void *)(1 * sizeof(glm::vec4)));

        glEnableVertexAttribArray(INSTANCE_POS_ATTR + 2);
        glVertexAttribPointer(INSTANCE_POS_ATTR + 2, 4, GL_FLOAT, GL_FALSE,
                              4 * sizeof(glm::vec4),
                              (void *)(2 * sizeof(glm::vec4)));

        glEnableVertexAttribArray(INSTANCE_POS_ATTR + 3);
        glVertexAttribPointer(INSTANCE_POS_ATTR + 3, 4, GL_FLOAT, GL_FALSE,
                              4 * sizeof(glm::vec4),
                              (void *)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(
            INSTANCE_POS_ATTR,
            1); // Iterate over each index in instances...don't skip any
        glVertexAttribDivisor(INSTANCE_POS_ATTR + 1, 1);
        glVertexAttribDivisor(INSTANCE_POS_ATTR + 2, 1);
        glVertexAttribDivisor(INSTANCE_POS_ATTR + 3, 1);

        // Unbind the arrays
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        assert(glGetError() == GL_NO_ERROR);
    }
}
void Shape::draw(const shared_ptr<Program> prog) const {
    DEBUG_MODE &&
        (cout << "(DRAW) Program: " << prog->getProgramName() << endl);
    for (int i = 0; i < obj_count; i++)

    {
        int h_pos, h_nor, h_tex;
        h_pos = h_nor = h_tex = -1;

        glBindVertexArray(vaoID[i]);
        // Bind position buffer
        h_pos = prog->getAttribute(
            vertexShaderAttributeNames[VERTEX_POSITION_ATTR]);
        GLSL::enableVertexAttribArray(h_pos);
        glBindBuffer(GL_ARRAY_BUFFER, posBufID[i]);
        glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        // Bind normal buffer
        h_nor =
            prog->getAttribute(vertexShaderAttributeNames[VERTEX_NORMAL_ATTR]);
        if (h_nor != -1 && norBufID[i] != 0) {
            GLSL::enableVertexAttribArray(h_nor);
            glBindBuffer(GL_ARRAY_BUFFER, norBufID[i]);
            glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0,
                                  (const void *)0);
        }

        if (texBufID[i] != 0) {
            // Bind texcoords buffer
            h_tex = prog->getAttribute(
                vertexShaderAttributeNames[VERTEX_TEXTURE_ATTR]);
            if (h_tex != -1 && texBufID[i] != 0) {
                GLSL::enableVertexAttribArray(h_tex);
                glBindBuffer(GL_ARRAY_BUFFER, texBufID[i]);
                glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0,
                                      (const void *)0);
            }
        }

        // Bind element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID[i]);

        // Draw
        glDrawElementsInstanced(GL_TRIANGLES, (int)eleBuf[i].size(),
                                GL_UNSIGNED_INT, (const void *)0,
                                instancedBuf[i].size());

        if (DEBUG_MODE)
            for (int j = 0; j < instancedBuf[i].size(); j++)
                cout << " Instance number: " << j
                     << " Mesh number inside OBJ: " << i << endl;

        // Disable and unbind
        if (h_tex != -1) {
            GLSL::disableVertexAttribArray(h_tex);
        }
        if (h_nor != -1) {
            GLSL::disableVertexAttribArray(h_nor);
        }
        GLSL::disableVertexAttribArray(h_pos);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}
