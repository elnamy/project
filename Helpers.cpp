//
//  Helpers.cpp
//  nectar_collector
//
//  Created by Jaron Schreiber on 4/20/22.
//

#include "Helpers.hpp"

#include <glad/glad.h>

#include <array>
#include <iostream>
#include <random>
#include <vector>
#include <iomanip>

#include "Camera.h"
#include "GLSL.h"
#include "Macros.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "WindowManager.h"
#include "stb_image.h"

using namespace std;

int usedTextures = 0;

double get_last_elapsed_time() {
    static double lasttime = glfwGetTime();
    double actualtime = glfwGetTime();
    double difference = actualtime - lasttime;
    lasttime = actualtime;
    return difference;
}

void getWingAnimations(std::map<int, Animation>& wing_animations)
{
    int kf = 0;

    wing_animations[kf] = Animation(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::quarter_pi<float>() * 0.8);

    kf++;
    wing_animations[kf] = Animation(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::quarter_pi<float>());

}

void createMaterialProgram(Programs program, const std::string &shaderDirectory,
                           const std::string &fragmentShaderName,
                           const std::string &vertexShaderName,
                           const std::string &tessControlName,
                           const std::string &tessEvalName,
                           GAME_BASE_MAP &gameMap) {
    std::shared_ptr<Program> shaderProgram = std::make_shared<Program>();
    shaderProgram->setProgramName(programNames[program]);
    shaderProgram->setVerbose(true);
    
    if (vertexShaderName.empty()) {
        shaderProgram->setShaderNames(
                                      shaderDirectory + DEFAULT_VERTEX_SHADER_FILE,
                                      shaderDirectory + fragmentShaderName,
                                      tessControlName.empty() ? "" : (shaderDirectory + tessControlName),
                                      tessEvalName.empty() ? "" : (shaderDirectory + tessEvalName));
    } else {
        shaderProgram->setShaderNames(
                                      shaderDirectory + vertexShaderName,
                                      shaderDirectory + fragmentShaderName,
                                      tessControlName.empty() ? "" : (shaderDirectory + tessControlName),
                                      tessEvalName.empty() ? "" : (shaderDirectory + tessEvalName));
    }
    
    shaderProgram->init();
        
    //for (int i = PROJECTION_UNIFORM; i < CAMERA_POSITION_UNIFORM; i++) {
    //    shaderProgram->addUniform(shaderUniformNames[i]);
    //}

    for (int i = PROJECTION_UNIFORM;
         i < sizeof(shaderUniformNames) / sizeof(shaderUniformNames[0]);
         i++) {
    shaderProgram->addUniform(shaderUniformNames[i]);
    }

    //for (int i = VERTEX_POSITION_ATTR; i < INSTANCE_POS_ATTR; i++) {
    //    shaderProgram->addAttribute(vertexShaderAttributeNames[i]);
    //}

    for (int i = VERTEX_POSITION_ATTR;
         i < sizeof(vertexShaderAttributeNames) /
                 sizeof(vertexShaderAttributeNames[0]);
         i++) {
    shaderProgram->addAttribute(vertexShaderAttributeNames[i]);
    }
    
    gameMap.insert(std::make_pair(
                                  program,
                                  std::make_tuple(shaderProgram, std::map<Objs, std::shared_ptr<Shape>>(),
                                                  std::map<Sampler2dTypes, GLuint>(),
                                                  std::map<Sampler2dTypes, GLuint>())));
}


void createBlurProgram(Programs program, const std::string& shaderDirectory,
    const std::string& fragmentShaderName,
    const std::string& vertexShaderName,
    const std::string& tessControlName,
    const std::string& tessEvalName,
    GAME_BASE_MAP& gameMap)
{
    std::shared_ptr<Program> shaderProgram = std::make_shared<Program>();
    shaderProgram->setProgramName(programNames[program]);
    shaderProgram->setVerbose(true);

    if (vertexShaderName.empty()) {
        shaderProgram->setShaderNames(
            shaderDirectory + DEFAULT_VERTEX_SHADER_FILE,
            shaderDirectory + fragmentShaderName,
            tessControlName.empty() ? "" : (shaderDirectory + tessControlName),
            tessEvalName.empty() ? "" : (shaderDirectory + tessEvalName));
    }
    else {
        shaderProgram->setShaderNames(
            shaderDirectory + vertexShaderName,
            shaderDirectory + fragmentShaderName,
            tessControlName.empty() ? "" : (shaderDirectory + tessControlName),
            tessEvalName.empty() ? "" : (shaderDirectory + tessEvalName));
    }

    shaderProgram->init();

    shaderProgram->addUniform("P");
    shaderProgram->addUniform("V");
    shaderProgram->addUniform("M");
    shaderProgram->addUniform("lightPosition");
    shaderProgram->addUniform("cameraPosition");
    shaderProgram->addUniform("sigma");
    shaderProgram->addUniform("blurSize");

    for (int i = VERTEX_POSITION_ATTR; i < INSTANCE_POS_ATTR; i++) {
        shaderProgram->addAttribute(vertexShaderAttributeNames[i]);
    }

    gameMap.insert(std::make_pair(
        program,
        std::make_tuple(shaderProgram, std::map<Objs, std::shared_ptr<Shape>>(),
            std::map<Sampler2dTypes, GLuint>(),
            std::map<Sampler2dTypes, GLuint>())));
}

void initParticleProgram(std::shared_ptr<Program> prog, const std::string& shaderDirectory)
{
    prog = make_shared<Program>();
    prog->setVerbose(true);
    prog->setShaderNames(
        shaderDirectory + "/lab10_vert.glsl",
        shaderDirectory + "/lab10_frag.glsl");
    if (!prog->init())
    {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    prog->addUniform("P");
    prog->addUniform("M");
    prog->addUniform("V");
    prog->addUniform("alphaTexture");
    prog->addAttribute("vertPos");
    prog->addAttribute("cPos");
}

void createParticleProgram(Programs program, const std::string& shaderDirectory,
    const std::string& fragmentShaderName,
    const std::string& vertexShaderName,
    const std::string& tessControlName,
    const std::string& tessEvalName,
    GAME_BASE_MAP& gameMap)
{
    std::shared_ptr<Program> shaderProgram = std::make_shared<Program>();
    shaderProgram->setProgramName(programNames[program]);
    shaderProgram->setVerbose(true);

    if (vertexShaderName.empty()) {
        shaderProgram->setShaderNames(
            shaderDirectory + DEFAULT_VERTEX_SHADER_FILE,
            shaderDirectory + fragmentShaderName,
            tessControlName.empty() ? "" : (shaderDirectory + tessControlName),
            tessEvalName.empty() ? "" : (shaderDirectory + tessEvalName));
    }
    else {
        shaderProgram->setShaderNames(
            shaderDirectory + vertexShaderName,
            shaderDirectory + fragmentShaderName,
            tessControlName.empty() ? "" : (shaderDirectory + tessControlName),
            tessEvalName.empty() ? "" : (shaderDirectory + tessEvalName));
    }

    shaderProgram->init();

    shaderProgram->addUniform("P");
    shaderProgram->addUniform("M");
    shaderProgram->addUniform("V");
    shaderProgram->addUniform("albedo");

    shaderProgram->addAttribute("cPos");
    shaderProgram->addAttribute("vertPos");

    gameMap.insert(std::make_pair(
        program,
        std::make_tuple(shaderProgram, std::map<Objs, std::shared_ptr<Shape>>(),
            std::map<Sampler2dTypes, GLuint>(),
            std::map<Sampler2dTypes, GLuint>())));
}

void initGeomForProgram(GAME_BASE_MAP &gameManager, Programs program,
                        string resourceDirectory, string objFilename,
                        Objs objName,
                        std::vector<glm::mat4> instancePositions) {
    shared_ptr<Shape> objShape = make_shared<Shape>();
    objShape->loadMesh(resourceDirectory + "/" + objFilename);
    objShape->resize();
    objShape->init(instancePositions);
    get<SHAPES>(gameManager[program]).insert(std::make_pair(objName, objShape));
}

void configureGLTextureParams(GAME_BASE_MAP &gameManager,
                              string resourceDirectory, string textureName,
                              Programs program, Sampler2dTypes textureType) {
    GLuint textureId;
    int width, height, channels;
    char filepath[1000];
    string str;
    unsigned char *data;
    
    str = resourceDirectory + "/" + textureName;
    strcpy(filepath, str.c_str());
    data = stbi_load(filepath, &width, &height, &channels, 4);
    glGenTextures(1, &textureId);
    glActiveTexture(GL_TEXTURE0 + usedTextures);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    get<TEXTURES>(gameManager[program])
        .insert(std::make_pair(textureType, textureId));
    get<GL_TEXTURES>(gameManager[program])
        .insert(std::make_pair(textureType, GL_TEXTURE0 + usedTextures++));
}

void exportTextureToUniform(GAME_BASE_MAP &gameManager, Programs program,
                            Sampler2dTypes textureType) {
    GLuint TexUniformName, GLTexNumber;
    
    TexUniformName = glGetUniformLocation(get<PROGRAM>(gameManager[program])->pid,
                                          sampler2dTypeNames[textureType]);
    GLTexNumber = get<GL_TEXTURES>(gameManager[program])[textureType];
    glUseProgram(get<PROGRAM>(gameManager[program])->pid);
    glUniform1i(TexUniformName, parseGLTextureNumber(GLTexNumber));
}
// returns 0 when glTextureNumber == GL_TEXTURE0 and increments every time
// glTextureNumber does
GLint parseGLTextureNumber(const int glTextureNumber) {
    return (glTextureNumber - GL_TEXTURE0);
}

float clamp(float n, float lower, float upper) {
    return std::max(lower, std::min(n, upper));
}

void print_map(std::map<string, std::shared_ptr<Shape>> const &m) {
    for (auto const &pair : m) {
        std::cout << "{" << pair.first << ": " << pair.second << "}\n";
    }
}

void print_vec(glm::vec3 vec) {
    cout << "x = " << vec.x << " y = " << vec.y << " z = " << vec.z << endl;
}

void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M) {
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE,
                       value_ptr(M->topMatrix()));
}

float RandomFloat(float a, float b) {
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;

    // td::cout << std::fixed << std::setprecision(4) << a + r << endl;
    return a + r;
}

bool randomBool(int probability, int outOf, int seed) {
    return probability > RandomFloat(0, outOf);
}

// *********** INSTANCING *************
// populates the instances of fences by drawing a circle of 50 fences
std::vector<glm::mat4> fenceInstances() {
    glm::mat4 add;
    std::vector<glm::mat4> res;
    for (int i = 0; i < 50; i++) {
        add = glm::mat4(1);
        // as big as I could make the circle and to never clip parts of the map
        add *= glm::scale(glm::mat4(1), glm::vec3(2.5, 2.5, 2.5));
        add *=
            glm::rotate(glm::mat4(1), (float)((float)(i / 50.0) * (2 * M_PI)),
                        glm::vec3(0, 1, 0));
        // the math checks out trust me
        add *= translate(glm::mat4(1), glm::vec3(0, .66, 15.9155));

        res.push_back(add);
    }
    return res;
}

std::vector<glm::mat4> treeInstances() {
    glm::mat4 add;
    float sX, sY, sZ, rY; // random SCALE XYZ and ROTATE Y
    std::vector<glm::mat4> res;
    for (int i = 0; i < 10; i++) {
        add = glm::mat4(1);
        // as big as I could make the circle and to never clip parts of the map
        sX = RandomFloat(.8, 1.2);
        sY = RandomFloat(.8, 1.2);
        sZ = RandomFloat(.8, 1.2);
        rY = RandomFloat(0, M_PI * 2);
        add *= glm::scale(glm::mat4(1), glm::vec3(4 * sX, 4 * sY, 4 * sZ));
        add *=
            glm::rotate(glm::mat4(1), (float)((float)(i / 10.0) * (2 * M_PI)),
                        glm::vec3(0, 1, 0));
        // the math checks out trust me
        add *= translate(glm::mat4(1), glm::vec3(0, 2, 8));

        add *= glm::rotate(glm::mat4(1), rY, glm::vec3(0, 1, 0));
        res.push_back(add);
    }
    return res;
}

std::vector<glm::mat4>
circularRandomizedInstances(int quantity, float scale, float floorheight,
                            float strataMin, float strataMax, float degreeMin,
                            float degreeMax) {
    strataMax = strataMax / scale; // Keeps things within bounds if the scale
                                   // is larger than the fences
    float randomScale;
    float randomRot;
    glm::mat4 add;
    std::vector<glm::mat4> res;
    for (int i = 0; i < quantity; i++) {
        add = glm::mat4(1);
        randomScale =
            RandomFloat(.6, 1.4) * scale; // Makes flowers look less uniform
        add *= glm::scale(glm::mat4(1),
                          glm::vec3(randomScale, randomScale, randomScale));
        // add *= glm::rotate(glm::mat4(1), (float)((float)(i / 50.0) *
        // (degree)),
        //   glm::vec3(0, 1, 0));
        add *= glm::rotate(glm::mat4(1), RandomFloat(degreeMin, degreeMax),
                           glm::vec3(0, 1, 0));
        add *= translate(glm::mat4(1),
                         glm::vec3(0, floorheight,
                                   clamp(abs(RandomFloat(strataMin, strataMax) *
                                             (scale - randomScale)),
                                         strataMin, strataMax)));

        randomRot = RandomFloat(
            -M_PI / 8, M_PI / 8); // slightly rotate either way so flowers
                                  // aren't facing same direction
        add *= glm::rotate(glm::mat4(1), randomRot, glm::vec3(0, 1, 0));

        // if (randomBool(6, 10, i)){
        res.push_back(add);
        //}
    }
    return res;
}
std::vector<glm::mat4> stoneInstances() {
    std::vector<glm::mat4> res;
    glm::mat4 add;

    float thisPosx = -29 + 6;
    float thisPosz = -29 - 1;
    for (int i = 0; i < 11; i++) {
        add = glm::mat4(1);
        add *= translate(glm::mat4(1),
                         glm::vec3(thisPosx + RandomFloat(0, .5), 0, thisPosz));

        add *= glm::scale(glm::mat4(1), glm::vec3(2.5, 2.5, 2.5));
        add *= rotate(glm::mat4(1), RandomFloat(0, M_PI), glm::vec3(0, 1, 0));

        // Model->scale(glm::vec3(2.5, 2, 2.5));
        // setModel(stoneProg, Model);
        // stone->draw(stoneProg);
        // Model->popMatrix();
        thisPosx += 3;
        thisPosz += 3;
        res.push_back(add);
    }
    return res;
}

void setPV(std::shared_ptr<Program> prog,
           std::shared_ptr<MatrixStack> Projection,
           std::shared_ptr<MatrixStack> View) {
    glUniformMatrix4fv(prog->getUniform(shaderUniformNames[PROJECTION_UNIFORM]), 1, GL_FALSE,
                       value_ptr(Projection->topMatrix()));
    glUniformMatrix4fv(prog->getUniform(shaderUniformNames[VIEW_UNIFORM]), 1, GL_FALSE,
                       value_ptr(View->topMatrix()));
}
