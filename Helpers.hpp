//
//  Helpers.hpp
//  nectar_collector
//
//  Created by Jaron Schreiber on 4/20/22.
//

#ifndef Helpers_hpp
#define Helpers_hpp

#include <glad/glad.h>
#include <stdio.h>

#include <array>
#include <iostream>
#include <random>
#include <vector>

#include "Camera.h"
#include "GLSL.h"
#include "Macros.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "WindowManager.h"
#include "Animation.h"

using namespace std;

typedef std::map<Objs, std::shared_ptr<Shape>> SHAPE_REF_TYPE;
typedef std::map<Sampler2dTypes, GLuint> TEXTURE_ID;
typedef std::map<Sampler2dTypes, GLuint> GL_TEXTURE_ID;
typedef std::map<Programs, tuple<std::shared_ptr<Program>, SHAPE_REF_TYPE,
                                 TEXTURE_ID, GL_TEXTURE_ID>>
    GAME_BASE_MAP;

// MARK: Random Number Generation
extern std::default_random_engine generator;
extern std::random_device
    rd; // Will be used to obtain a seed for the random number engine

void createMaterialProgram(Programs program, const std::string &shaderDirectory,
                           const std::string &fragmentShaderName,
                           const std::string &vertexShaderName,
                           const std::string &tessControlName,
                           const std::string &tessEvalName,
                           GAME_BASE_MAP &gameMap);

void createBlurProgram(Programs program, const std::string& shaderDirectory,
    const std::string& fragmentShaderName,
    const std::string& vertexShaderName,
    const std::string& tessControlName,
    const std::string& tessEvalName,
    GAME_BASE_MAP& gameMap);

void initParticleProgram(std::shared_ptr<Program> prog, const std::string& shaderDirectory);

void createParticleProgram(Programs program, const std::string& shaderDirectory,
    const std::string& fragmentShaderName,
    const std::string& vertexShaderName,
    const std::string& tessControlName,
    const std::string& tessEvalName,
    GAME_BASE_MAP& gameMap);

void configureGLTextureParams(
    GAME_BASE_MAP &gameManager, string resourceDirectory,
    string textureName, // don't include slash, function will handle
    Programs program, Sampler2dTypes textureType);

void exportTextureToUniform(GAME_BASE_MAP &gameManager, Programs program,
                            Sampler2dTypes textureType);

void initGeomForProgram(GAME_BASE_MAP &gameManager, Programs program,
                        string resourceDirectory, string objFilename,
                        Objs objName, std::vector<glm::mat4> instancePositions);

// MARK: Animation
double get_last_elapsed_time();

void getWingAnimations(std::map<int, Animation>& wing_animations);

// MARK: Textures

extern int usedTextures;

GLint parseGLTextureNumber(const int glTextureNumber);

// MARK: Miscellaneous

float clamp(float n, float lower, float upper);

void print_map(std::map<string, std::shared_ptr<Shape>> const &m);

void print_vec(glm::vec3 vec);

void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M);

// MARK: Instancing

std::vector<glm::mat4> fenceInstances();
std::vector<glm::mat4> treeInstances();
std::vector<glm::mat4> stoneInstances();

float RandomFloat(float a, float b, string ran, int seed = 0);

bool randomBool(int probability, int outOf, int seed = 0);

std::vector<glm::mat4> circularRandomizedInstances(int quantity, float scale,
                                                   float floorheight = .5, float strataMin = 0,
                                                   float strataMax = 15.9155, float degreeMin = 0,
                                                   float degreeMax = 2 * M_PI);



void setPV(std::shared_ptr<Program> prog,
           std::shared_ptr<MatrixStack> Projection,
           std::shared_ptr<MatrixStack> View);
#endif /* Helpers_hpp */
