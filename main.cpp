/*
 ZJ Wood, Dunn, Eckhardt CPE 471 Lab base code
 */
#include <glad/glad.h>

#include <array>
#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"
#include "stb_image.h"

#include "Animation.h"
#include "Camera.h"
#include "FBOManager.h"
#include "Helpers.hpp"
#include "Macros.h"
#include "RenderGeoHelpers.hpp"
#include "Texture.h"
#include "particleSys.h"

#define STB_IMAGE_IMPLEMENTATION

#include "Shape.h"
#include "WindowManager.h"
// value_ptr for glm
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

using namespace std;
using namespace glm;

// MARK: Global usage

std::shared_ptr<Texture> texture;
std::shared_ptr<Program> partProg;
particleSys *thePartSystem;
std::map<int, Animation> wing_animations;
unsigned int kf = 0;

Camera my_cam(glm::vec3(0, 1, 10), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0),
              10.0f);

ma_engine engine;

class Application : public EventCallbacks {
  public:
    WindowManager *windowManager = nullptr;

    // All shader programs live here...[Program
    // Name][PROGRAM|SHAPES|TEXTURES|GL_TEXTURES\OCTREES]
    GAME_BASE_MAP gameManager;

    FBOManager fboManager;

    // MARK: Lights
    glm::vec3 mainLightPosition = glm::vec3(-100, -100, -100);

    float acc = ACC;

    bool zoeMode = false;

    bool A_KeyPressed;
    bool W_KeyPressed;
    bool D_KeyPressed;
    bool sunsetEnabled;

    // ************************ Rendering Functions ************************

    // MARK: 1. Create Materials and Enable Render Settings
    void initPrograms(const std::string &resourceDirectory) {
        GLSL::checkVersion();

        // Set background color.
        glClearColor(0.305f, 0.3294f, 0.50588f, 1.0f);

        // Enable z-buffer test.
        glEnable(GL_DEPTH_TEST);

        // Enable transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glFrontFace(GL_CW);

        createMaterialProgram(BEE_PROGRAM, resourceDirectory + SHADER_DIR,
                              "/bee_fragment_shader.glsl", "", "", "",
                              gameManager);

        getWingAnimations(wing_animations);
        createBlurProgram(BEE_WINGS_PROGRAM, resourceDirectory + SHADER_DIR,
                          "/bee_wing_fragment_shader.glsl", "", "", "",
                          gameManager);

        createMaterialProgram(SKY_PROGRAM, resourceDirectory + SHADER_DIR,
                              "/sky_fragment_shader.glsl", "", "", "",
                              gameManager);
        //
        createMaterialProgram(SUNSET_PROGRAM, resourceDirectory + SHADER_DIR,
                              "/sky_fragment_shader.glsl", "", "", "",
                              gameManager);

        createMaterialProgram(GROUND_PROGRAM, resourceDirectory + SHADER_DIR,
                              "/ground_fragment_shader.glsl", "", "", "",
                              gameManager);

        createMaterialProgram(FENCE_PROGRAM, resourceDirectory + SHADER_DIR,
                              "/bee_fragment_shader.glsl", "", "", "",
                              gameManager);

        createMaterialProgram(TREES_PROGRAM, resourceDirectory + SHADER_DIR,
                              "/bee_fragment_shader.glsl", "", "", "",
                              gameManager);
        createMaterialProgram(CROCUS_PROGRAM, resourceDirectory + SHADER_DIR,
                              "/bee_fragment_shader.glsl", "", "", "",
                              gameManager);
        createMaterialProgram(ANEMONE_PROGRAM, resourceDirectory + SHADER_DIR,
                              "/bee_fragment_shader.glsl", "", "", "",
                              gameManager);
        createMaterialProgram(DAFFODIL_PROGRAM, resourceDirectory + SHADER_DIR,
                              "/bee_fragment_shader.glsl", "", "", "",
                              gameManager);
        createMaterialProgram(WELL_PROGRAM, resourceDirectory + SHADER_DIR,
                              "/bee_fragment_shader.glsl", "", "", "",
                              gameManager);
        createMaterialProgram(POND_PROGRAM, resourceDirectory + SHADER_DIR,
                              "/bee_fragment_shader.glsl", "", "", "",
                              gameManager);
        createMaterialProgram(TWBERRIES_PROGRAM, resourceDirectory + SHADER_DIR,
                              "/bee_fragment_shader.glsl", "", "", "",
                              gameManager);
        createMaterialProgram(STONEP_PROGRAM, resourceDirectory + SHADER_DIR,
                              "/bee_fragment_shader.glsl", "", "", "",
                              gameManager);

        createParticleProgram(PARTICLE_PROGRAM, resourceDirectory + SHADER_DIR,
                              "/lab10_frag.glsl", "/lab10_vert.glsl", "", "",
                              gameManager);

        // **************** FBO ******************
        fboManager.createFrameBufferProgram(resourceDirectory + SHADER_DIR,
                                            "/fbo_pass_frag.glsl",
                                            "/fbo_pass_vert.glsl", gameManager);
    }

    // MARK: 2. Import OBJ files
    void initGeomPerProgram(const std::string &resourceDirectory) {
        // **************** BEE ******************
        initGeomForProgram(
            gameManager, BEE_PROGRAM, resourceDirectory, "bee_body.obj",
            BEE_BODY_OBJ,
            {
                glm::scale(glm::mat4(1), glm::vec3(0.01, 0.01, 0.01)),
            });

        initGeomForProgram(
            gameManager, BEE_WINGS_PROGRAM, resourceDirectory, "bee_wings.obj",
            BEE_WINGS_OBJ,
            {
                glm::scale(glm::mat4(1), glm::vec3(0.01, 0.01, 0.01)),
            });

        // **************** SKY ******************
        initGeomForProgram(gameManager, SKY_PROGRAM, resourceDirectory,
                           "sphere.obj", SKY_SPHERE_OBJ, {});
        // sunset
        initGeomForProgram(gameManager, SUNSET_PROGRAM, resourceDirectory,
                           "sphere.obj", SKY_SPHERE_OBJ, {});

        // **************** GROUND ******************
        initGeomForProgram(gameManager, GROUND_PROGRAM, resourceDirectory,
                           "circle_filled.obj", GROUND_OBJ, {});

        // **************** FENCE ******************
        initGeomForProgram(gameManager, FENCE_PROGRAM, resourceDirectory,
                           "WoodFence.obj", FENCE_OBJ, fenceInstances());

        // to take up 1/4 of the screen with a particular instance, you would
        // say fullRotation/4
        float fullRotation = (M_PI / 3) * 2;

        // **************** TREE ******************
        initGeomForProgram(gameManager, TREES_PROGRAM, resourceDirectory,
                           "mountain_ash_1.obj", TREES_OBJ, treeInstances());

        // **************** BUSH ******************
        initGeomForProgram(
            gameManager, TWBERRIES_PROGRAM, resourceDirectory,
            "mountain_ash_with_berries_2.obj", TWBERRIES_OBJ,
            {
                //
                circularRandomizedInstances(10, 2, .8, 12, 14)
                // glm::translate(glm::mat4(1), glm::vec3(10, 0, -5))
            });
        // **************** CROCUS_FLOWER ******************
        initGeomForProgram(gameManager, CROCUS_PROGRAM, resourceDirectory,
                           "crocus_flower.obj", CROCUS_OBJ,
                           {circularRandomizedInstances(
                               50, 3, .3, 5, 28, -fullRotation / 2.6,
                               -(fullRotation / 2.6) + fullRotation / 2)});
        // **************** ANEMONE_FLOWER ******************
        initGeomForProgram(gameManager, ANEMONE_PROGRAM, resourceDirectory,
                           "anemone_flower.obj", ANEMONE_OBJ,
                           {circularRandomizedInstances(25, 3, .5, 0, 3)

                           });

        // **************** DAFFODIL_FLOWER ******************

        initGeomForProgram(gameManager, DAFFODIL_PROGRAM, resourceDirectory,
                           "Daffodil_flower.obj", // YELLOW FLOWER
                           DAFFODIL_OBJ,
                           {circularRandomizedInstances(40, 3, .5, 0, 6)

                           });
        // **************** WELL ******************
        initGeomForProgram(gameManager, WELL_PROGRAM, resourceDirectory,
                           "waterWell.obj", WELL_OBJ,
                           {glm::translate(glm::mat4(1), glm::vec3(4, 0, 0))});
        // **************** POND ******************
        initGeomForProgram(gameManager, POND_PROGRAM, resourceDirectory,
                           "Pond.obj", POND_OBJ,
                           {glm::translate(glm::mat4(1), glm::vec3(6, 0, 6))});

        // **************** PATH ******************
        initGeomForProgram(gameManager, STONEP_PROGRAM, resourceDirectory,
                           "stone_path.obj", STONE_OBJ, stoneInstances());

        // **************** PARTICLE_SYSTEM ******************
        thePartSystem = new particleSys(glm::vec3(0, 0, 0), 0.03);
        // thePartSystem = new particleSys(glm::vec3(-8, 2, 5));
        thePartSystem->gpuSetup();
        glPointSize(9.0f);

        // **************** FBO ******************
        fboManager.initQuad();
    }

    // MARK: 3. Import Texture Images
    void initTexturesPerProgram(const std::string &resourceDirectory) {
        // **************** BEE ******************
        configureGLTextureParams(gameManager, resourceDirectory,
                                 "black_and_yellow.jpg", BEE_PROGRAM, ALBEDO);
        exportTextureToUniform(gameManager, BEE_PROGRAM, ALBEDO);

        configureGLTextureParams(gameManager, resourceDirectory, "blueWing.jpg",
                                 BEE_WINGS_PROGRAM, ALBEDO);
        exportTextureToUniform(gameManager, BEE_WINGS_PROGRAM, ALBEDO);

        // **************** SKY ******************
        configureGLTextureParams(gameManager, resourceDirectory, "sky.jpg",
                                 SKY_PROGRAM, ALBEDO);
        exportTextureToUniform(gameManager, SKY_PROGRAM, ALBEDO);
        // **************** SUNSET ******************
        configureGLTextureParams(gameManager, resourceDirectory,
                                 "sunset_sky.jpeg", SUNSET_PROGRAM, ALBEDO);
        exportTextureToUniform(gameManager, SUNSET_PROGRAM, ALBEDO);

        // **************** GROUND ******************
        configureGLTextureParams(gameManager, resourceDirectory, "grass.jpg",
                                 GROUND_PROGRAM, ALBEDO);
        exportTextureToUniform(gameManager, GROUND_PROGRAM, ALBEDO);

        // **************** FENCE ******************
        configureGLTextureParams(gameManager, resourceDirectory,
                                 "woodFenceAlbedo.png", FENCE_PROGRAM, ALBEDO);
        exportTextureToUniform(gameManager, FENCE_PROGRAM, ALBEDO);

        // **************** TREE ******************
        configureGLTextureParams(gameManager, resourceDirectory,
                                 "mountain_ash_bushes_texture_atlas.png",
                                 TREES_PROGRAM, ALBEDO);
        exportTextureToUniform(gameManager, TREES_PROGRAM, ALBEDO);
        configureGLTextureParams(gameManager, resourceDirectory,
                                 "mountain_ash_bushes_texture_atlas.png",
                                 TWBERRIES_PROGRAM, ALBEDO);
        exportTextureToUniform(gameManager, TWBERRIES_PROGRAM, ALBEDO);
        // **************** CROCUS_FLOWER ******************
        configureGLTextureParams(gameManager, resourceDirectory,
                                 "crocus_flower.jpg", CROCUS_PROGRAM, ALBEDO);
        exportTextureToUniform(gameManager, CROCUS_PROGRAM, ALBEDO);
        // **************** ANEMONE_FLOWER ******************
        configureGLTextureParams(gameManager, resourceDirectory,
                                 "anemone_flower.jpg", ANEMONE_PROGRAM, ALBEDO);
        exportTextureToUniform(gameManager, ANEMONE_PROGRAM, ALBEDO);
        // **************** DAFFODIL_FLOWER ******************
        configureGLTextureParams(gameManager, resourceDirectory,
                                 "Daffodil_Flower.jpg", DAFFODIL_PROGRAM,
                                 ALBEDO);
        exportTextureToUniform(gameManager, DAFFODIL_PROGRAM, ALBEDO);
        // **************** WELL ******************
        configureGLTextureParams(gameManager, resourceDirectory, "Barrel.png",
                                 WELL_PROGRAM, ALBEDO);
        exportTextureToUniform(gameManager, WELL_PROGRAM, ALBEDO);
        // **************** POND ******************
        configureGLTextureParams(gameManager, resourceDirectory,
                                 "CartoonPond_Base_ColorWithAO.png",
                                 POND_PROGRAM, ALBEDO);
        exportTextureToUniform(gameManager, POND_PROGRAM, ALBEDO);
        // **************** STONE_PATH ******************
        configureGLTextureParams(gameManager, resourceDirectory,
                                 "stone_path.jpg", STONEP_PROGRAM, ALBEDO);
        exportTextureToUniform(gameManager, STONEP_PROGRAM, ALBEDO);

        // **************** PARTICLE_SYSTEM ******************
        configureGLTextureParams(gameManager, resourceDirectory, "alpha.bmp",
                                 PARTICLE_PROGRAM, ALBEDO);
        exportTextureToUniform(gameManager, PARTICLE_PROGRAM, ALBEDO);

        // **************** FBO ******************
        GLuint fbo1 = fboManager.getNextAvailableFBOId();
        GLuint fbotex1 = fboManager.getNextAvailableTextureId();
        GLuint depthBuf1 = fboManager.getNextAvailableDepthId();

        fboManager.initializeFBO(windowManager, fbo1, fbotex1);
        fboManager.initializeDepthBuffer(windowManager, depthBuf1);

        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers);

        GLuint fbo2 = fboManager.getNextAvailableFBOId();
        GLuint fbotex2 = fboManager.getNextAvailableTextureId();

        fboManager.initializeFBO(windowManager, fbo2, fbotex2);
    }

    // MARK: 4. Render - Manipulate objects
    void render() {
        double frametime = get_last_elapsed_time();
        static float originTime = 0.0;
        originTime += frametime;

        if (zoeMode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Matrix Stacks
        auto Projection = make_shared<MatrixStack>();
        auto View = make_shared<MatrixStack>();
        auto Model = make_shared<MatrixStack>();

        // **************** VIEWPORT ******************

        // Get current frame buffer size.
        int width, height;

        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        glViewport(0, 0, width, height);

        // Frame Buffer Object Rendering
        glBindFramebuffer(GL_FRAMEBUFFER, fboManager.getFBOIdFor(0));

        // Clear framebuffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspect = width / (float)height;

        Projection->pushMatrix();
        Projection->perspective(45.0f, width, (float)height, 0.01f, 100.0f);

        // Move
        my_cam.updateCamera(frametime, View);

        // **************** SKY ******************
        if (sunsetEnabled) {
            std::shared_ptr<Program> skyProg = renderSky(
                gameManager, Model, Projection, View, my_cam, "SUNSET_PROGRAM");
        } else {
            std::shared_ptr<Program> skyProg = renderSky(
                gameManager, Model, Projection, View, my_cam, "SKY_PROGRAM");
        }

        // **************** BEE ******************
        std::shared_ptr<Program> beeProg =
            renderBee(gameManager, Model, Projection, View, mainLightPosition,
                      my_cam, A_KeyPressed);
        std::shared_ptr<Program> beeWingsProg = renderBeeWings(
            gameManager, Model, Projection, View, mainLightPosition, width,
            wing_animations, my_cam, A_KeyPressed);

        //**************** GROUND ******************
        std::shared_ptr<Program> groundProg = renderGround(
            gameManager, Model, Projection, View, mainLightPosition, my_cam);

        // **************** FENCE ******************
        std::shared_ptr<Program> fenceProg = renderFence(
            gameManager, Model, Projection, View, mainLightPosition, my_cam);

        // **************** TREES ******************
        std::shared_ptr<Program> treeProg = renderTrees(
            gameManager, Model, Projection, View, mainLightPosition, my_cam);

        // **************** CROCUS_FLOWER ******************
        std::shared_ptr<Program> crocusProg = renderCrocus(
            gameManager, Model, Projection, View, mainLightPosition, my_cam);

        // **************** ANEMONE_FLOWER ******************
        std::shared_ptr<Program> anemoneProg = renderAnemone(
            gameManager, Model, Projection, View, mainLightPosition, my_cam);

        // **************** DAFFODIL_FLOWER ******************
        std::shared_ptr<Program> daffodilProg = renderDaffodil(
            gameManager, Model, Projection, View, mainLightPosition, my_cam);

        // **************** WELL ******************
        std::shared_ptr<Program> wellProg = renderWell(
            gameManager, Model, Projection, View, mainLightPosition, my_cam);

        // **************** POND ******************
        std::shared_ptr<Program> pondProg = renderPond(
            gameManager, Model, Projection, View, mainLightPosition, my_cam);

        // **************** TWBERRIES ******************
        std::shared_ptr<Program> twBerriesProg = renderTwBerries(
            gameManager, Model, Projection, View, mainLightPosition, my_cam);

        // **************** STONE_PATH ******************
        std::shared_ptr<Program> stonePathProg = renderStonePath(
            gameManager, Model, Projection, View, mainLightPosition, my_cam);

        // **************** PARTICLE_TEST ******************
        std::shared_ptr<Program> partProg =
            renderParticles(gameManager, Model, Projection, View,
                            mainLightPosition, thePartSystem, my_cam);

        // **************** CLEANUP ******************

        Projection->popMatrix();

        // **************** FBO ******************

        glBindFramebuffer(GL_FRAMEBUFFER, ONSCREEN);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        std::shared_ptr<Program> fboProg =
            get<PROGRAM>(gameManager[FBO_PROGRAM]);
        fboManager.ProcessDrawTex(fboProg, 0);
    }

    // ************************ Event Callbacks ************************

    void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                     int mods) {
        // Window Escape
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        // Bee Throttle
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
            my_cam.setThrottle(true);
        }
        if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
            my_cam.setThrottle(false);
        }

        // Camera Control
        if (key == GLFW_KEY_W && action == GLFW_PRESS) {
            // mycam.w = 0;
            my_cam.setForward(true);
            A_KeyPressed = true;
            W_KeyPressed = true;
            setRotationAngle(M_PI / 6, vec3(1, 0, 0));
        }
        if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
            // mycam.w = 0;
            my_cam.setForward(false);
            A_KeyPressed = false;
        }
        if (key == GLFW_KEY_S && action == GLFW_PRESS) {
            // mycam.s = 1;
            my_cam.setBackward(true);
        }
        if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
            // mycam.s = 0;
            my_cam.setBackward(false);
        }
        if (key == GLFW_KEY_A && action == GLFW_PRESS) {
            // mycam.a = 1;
            my_cam.setLeft(true);
            A_KeyPressed = true;
            setRotationAngle(0.52, vec3(0, 0, 1));
        }
        if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
            // mycam.a = 0;
            my_cam.setLeft(false);
            A_KeyPressed = false;
        }
        if (key == GLFW_KEY_D && action == GLFW_PRESS) {
            // mycam.d = 1;
            my_cam.setRight(true);
            A_KeyPressed = true;
            setRotationAngle(-0.52, vec3(0, 0, 1));
        }
        if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
            // mycam.d = 0;
            my_cam.setRight(false);
            A_KeyPressed = false;
        }

        if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
            zoeMode = !zoeMode;
        }

        if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
            sunsetEnabled = true;
        }
        if (key == GLFW_KEY_0 && action == GLFW_RELEASE) {
            sunsetEnabled = false;
        }
    }

    // callback for the mouse when clicked move the triangle when helper
    // functions written
    void mouseCallback(GLFWwindow *window, int button, int action, int mods) {
        double posX, posY;
        if (action == GLFW_PRESS) {
            glfwGetCursorPos(window, &posX, &posY);
            //            std::cout << "Pos X " << posX <<  " Pos Y " << posY <<
            //            std::endl;
        }
    }

    void scrollCallback(GLFWwindow *window, double deltaX, double deltaY) {
        if (my_cam.getPitch() > PITCH_EDGE) {
            my_cam.setPitch(PITCH_EDGE);
        } else if (my_cam.getPitch() < -PITCH_EDGE) {
            my_cam.setPitch(-PITCH_EDGE);
        } else {
            my_cam.incrementPitch(
                (SCROLL_SPEED * glm::pi<float>() / glm::radians(180.0)) *
                deltaY);
        }
        my_cam.incrementYaw(
            ((2 * SCROLL_SPEED * glm::pi<float>()) / glm::radians(180.0)) *
            deltaX);
    }

    void cursorCallback(GLFWwindow *window, double xpos, double ypos) {
        // Pending use...
    }

    // if the window is resized, capture the new size and reset the viewport
    void resizeCallback(GLFWwindow *window, int in_width, int in_height) {
        // get the window size - may be different then pixels for retina
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
    }
};

// ************************ Main ************************

int main(int argc, char **argv) {
    std::string resourceDir =
        DEFAULT_RESOURCES_FOLDER; // Where the resources are loaded from
    if (argc >= 2) {
        resourceDir = argv[1];
    }

    // Create application
    Application *application = new Application();

    // C++ Randomization
    srand(963470346);

    // Establish window size and event callbacks
    WindowManager *windowManager = new WindowManager();
    windowManager->init(1920, 1080);
    windowManager->setEventCallbacks(application);
    application->windowManager = windowManager;

    // Initialize scene.
    application->initPrograms(resourceDir);
    application->initGeomPerProgram(resourceDir + OBJ_DIR);
    application->initTexturesPerProgram(resourceDir + TEXTURE_DIR);

    ma_result result;

    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize audio engine.");
        return -1;
    }

    ma_engine_play_sound(&engine, "../resources/sounds/bee_calm.wav", NULL);

    // Loop until the user closes the window.
    while (!glfwWindowShouldClose(windowManager->getHandle())) {
        // Render scene.
        application->render();

        // Swap front and back buffers.
        glfwSwapBuffers(windowManager->getHandle());
        // Poll for and process events.
        glfwPollEvents();
    }

    ma_engine_uninit(&engine);

    // Quit program.
    windowManager->shutdown();

    return 0;
}
