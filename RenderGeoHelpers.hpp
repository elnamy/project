//
//  RenderGeoHelpers.hpp
//  nectar_collector
//
//  Created by Jaron Schreiber on 5/11/22.
//

#ifndef RenderGeoHelpers_hpp
#define RenderGeoHelpers_hpp

#include <stdio.h>

#include "Helpers.hpp"
#include "Program.h"
#include "particleSys.h"

void setRotationAngle(float angle, vec3 axis);

std::shared_ptr<Program> renderSky(GAME_BASE_MAP gameManager,
                                   shared_ptr<MatrixStack> Model,
                                   shared_ptr<MatrixStack> Projection,
                                   shared_ptr<MatrixStack> View, Camera my_cam,
                                   string program);

std::shared_ptr<Program>
renderBee(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
          shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
          glm::vec3 mainLightPosition, Camera my_cam, bool isKeyPressed);

std::shared_ptr<Program>
renderBeeWings(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
               shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
               glm::vec3 mainLightPosition, int width,
               std::map<int, Animation> wing_animations, Camera my_cam,
               bool isKeyPressed);

std::shared_ptr<Program>
renderGround(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
             shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
             glm::vec3 mainLightPosition, Camera my_cam);

std::shared_ptr<Program>
renderFence(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
            shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
            glm::vec3 mainLightPosition, Camera my_cam);

std::shared_ptr<Program>
renderTrees(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
            shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
            glm::vec3 mainLightPosition, Camera my_cam);

std::shared_ptr<Program>
renderCrocus(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
             shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
             glm::vec3 mainLightPosition, Camera my_cam);

std::shared_ptr<Program>
renderAnemone(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
              shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
              glm::vec3 mainLightPosition, Camera my_cam);

std::shared_ptr<Program>
renderDaffodil(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
               shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
               glm::vec3 mainLightPosition, Camera my_cam);

std::shared_ptr<Program> renderWell(GAME_BASE_MAP gameManager,
                                    shared_ptr<MatrixStack> Model,
                                    shared_ptr<MatrixStack> Projection,
                                    shared_ptr<MatrixStack> View,
                                    glm::vec3 mainLightPosition, Camera my_cam);

std::shared_ptr<Program> renderPond(GAME_BASE_MAP gameManager,
                                    shared_ptr<MatrixStack> Model,
                                    shared_ptr<MatrixStack> Projection,
                                    shared_ptr<MatrixStack> View,
                                    glm::vec3 mainLightPosition, Camera my_cam);

std::shared_ptr<Program> renderTwBerries(GAME_BASE_MAP gameManager,
                                         shared_ptr<MatrixStack> Model,
                                         shared_ptr<MatrixStack> Projection,
                                         shared_ptr<MatrixStack> View,
                                         glm::vec3 mainLightPosition,
                                         Camera my_cam);

std::shared_ptr<Program> renderStonePath(GAME_BASE_MAP gameManager,
                                         shared_ptr<MatrixStack> Model,
                                         shared_ptr<MatrixStack> Projection,
                                         shared_ptr<MatrixStack> View,
                                         glm::vec3 mainLightPosition,
                                         Camera my_cam);

std::shared_ptr<Program>
renderParticles(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
                shared_ptr<MatrixStack> Projection,
                shared_ptr<MatrixStack> View, glm::vec3 mainLightPosition,
                particleSys *thePartSystem, Camera my_cam);
#endif /* RenderGeoHelpers_hpp */
