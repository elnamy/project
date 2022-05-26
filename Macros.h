#pragma once

#ifndef Macros_h
#define Macros_h

// MARK: These may be changed occasionally

// Materials - different fragment shader for each
enum Programs {
    FBO_PROGRAM,
    BEE_PROGRAM,
    BEE_WINGS_PROGRAM,
    SKY_PROGRAM,
    SUNSET_PROGRAM,
    GROUND_PROGRAM,
    FENCE_PROGRAM,
    TREES_PROGRAM,
    CROCUS_PROGRAM,
    ANEMONE_PROGRAM,
    DAFFODIL_PROGRAM,
    WELL_PROGRAM,
    POND_PROGRAM,
    TWBERRIES_PROGRAM,
    STONEP_PROGRAM,
    PARTICLE_PROGRAM
};
static const char *const programNames[] = {
    "FBO_PROGRAM",     "BEE_PROGRAM",       "BEE_WINGS_PROGRAM",
    "SKY_PROGRAM",     "SUNSET_PROGRAM",    "GROUND_PROGRAM",
    "FENCE_PROGRAM",   "TREES_PROGRAM",     "CROCUS_PROGRAM",
    "ANEMONE_PROGRAM", "DAFFODIL_PROGRAM",  "WELL_PROGRAM",
    "POND_PROGRAM",    "TWBERRIES_PROGRAM", "STONEP_PROGRAM",
    "PARTICLE_PROGRAM"};

// Objects - different .obj for each
enum Objs {
    BEE_BODY_OBJ,
    BEE_WINGS_OBJ,
    SKY_SPHERE_OBJ,
    GROUND_OBJ,
    FENCE_OBJ,
    TREES_OBJ,
    CROCUS_OBJ,
    ANEMONE_OBJ,
    DAFFODIL_OBJ,
    WELL_OBJ,
    POND_OBJ,
    TWBERRIES_OBJ,
    STONE_OBJ,
    PARTICLE_OBJ
};
static const char *const objNames[] = {
    "bee_body_obj", "bee_wings_obj", "sky_sphere_obj", "ground_obj",
    "fence_obj",    "trees_obj",     "crocus_obj",     "anemone_obj",
    "daffodil_obj", "well_obj",      "pond_obj",       "twberries_obj",
    "stone_obj",    "particle_obj"

};

// Frame Buffers
enum FrameBuffer {
    ONSCREEN,
};

// GROUND
#define GROUND_SIZE 25.0

// DEBUG
#define DEBUG_MODE false

// ***********************************************

// MARK: These should NOT be changed

// Material Program Setup
#define DEFAULT_VERTEX_SHADER_FILE "/default_vertex_shader.glsl"
#define DEFAULT_RESOURCES_FOLDER "../resources"
#define OBJ_DIR "/objs"
#define SHADER_DIR "/shaders"
#define TEXTURE_DIR "/textures"

enum CentralProgramMap { PROGRAM, SHAPES, TEXTURES, GL_TEXTURES };

// Uniform Sampler2D names
enum Sampler2dTypes { ALBEDO, NORMAL, SPECULAR };
static const char *const sampler2dTypeNames[] = {
    "albedo",
    "normal",
    "specular",
};

// Attributes that go into vertex shader
enum VertexShaderAttributes {
    VERTEX_POSITION_ATTR,
    VERTEX_NORMAL_ATTR,
    VERTEX_TEXTURE_ATTR,
    INSTANCE_POS_ATTR
};
static const char *const vertexShaderAttributeNames[] = {
    "vertPos", "vertNor", "vertTex", "instancePos"};

// Uniforms that go into vertex/frag shaders
enum ShaderUniforms {
    PROJECTION_UNIFORM,
    VIEW_UNIFORM,
    MODEL_UNIFORM,
    LIGHT_POSITION_UNIFORM,
    CAMERA_POSITION_UNIFORM,

    FTIME_UNIFORM,
    FBO_TEX_BUFFER_UNIFORM
};
static const char *const shaderUniformNames[] = {"P",
                                                 "V",
                                                 "M",
                                                 "lightPosition",
                                                 "cameraPosition",
                                                 "fTime",
                                                 "fboTextureBuffer"};

// Occpancy Map
#define OCCUPANCY_MAP_WIDTH 400
#define OCCUPANCY_MAP_HEIGHT 400

// Random
#define GRAVITY .00
#define M_PI 3.14
#define M_PI_2 1.57

// Camera Values
#define PITCH_EDGE 89.0
#define SCROLL_SPEED 1
#define ACC -9.8

// OpenGL
#define SINGLE_UNIFORM_COUNT 1

#endif /* Macros_h */
