//
//  RenderGeoHelpers.cpp
//  nectar_collector
//
//  Created by Jaron Schreiber on 5/11/22.
//

#include "RenderGeoHelpers.hpp"
#include "particleSys.h"

float rotationAngle;
vec3 rotatingAxis;
float ROTATE_SPEED = 5.0;
float ROTATE_SPEED2 = 0.05;
float rotation = 0;
float rotation2 = 0;

std::shared_ptr<Program> renderSky(GAME_BASE_MAP gameManager,
                                   shared_ptr<MatrixStack> Model,
                                   shared_ptr<MatrixStack> Projection,
                                   shared_ptr<MatrixStack> View, Camera my_cam,
                                   string program) {
    std::shared_ptr<Program> skyProg;
    std::map<Objs, std::shared_ptr<Shape>> skyShapes;
    if (program == "SKY_PROGRAM") {
        skyProg = get<PROGRAM>(gameManager[SKY_PROGRAM]);
        skyShapes = get<SHAPES>(gameManager[SKY_PROGRAM]);
    } else {
        skyProg = get<PROGRAM>(gameManager[SUNSET_PROGRAM]);
        skyShapes = get<SHAPES>(gameManager[SUNSET_PROGRAM]);
    }
    skyProg->bind();

    Model->pushMatrix();
    Model->loadIdentity();
    Model->translate(my_cam.getEye());
    rotation += ROTATE_SPEED * get_last_elapsed_time();
    Model->rotate(glm::half_pi<float>(), glm::vec3(1, 0, 0));
    Model->rotate(rotation, vec3(0, 0, 1));
    Model->scale(glm::vec3(10, 10, 10));

    glUniformMatrix4fv(skyProg->getUniform(shaderUniformNames[MODEL_UNIFORM]),
                       SINGLE_UNIFORM_COUNT, GL_FALSE,
                       value_ptr(Model->topMatrix()));

    setPV(skyProg, Projection, View);

    glUniform3fv(
        skyProg->getUniform(shaderUniformNames[CAMERA_POSITION_UNIFORM]),
        SINGLE_UNIFORM_COUNT, &my_cam.getEye()[0]);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    std::shared_ptr<Shape> skysphere = skyShapes[SKY_SPHERE_OBJ];
    skysphere->draw(skyProg);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    Model->popMatrix();
    skyProg->unbind();

    return skyProg;
}

void setRotationAngle(float angle, vec3 axis) {
    rotationAngle = angle;
    rotatingAxis = axis;
}

std::shared_ptr<Program>
renderBee(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
          shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
          glm::vec3 mainLightPosition, Camera my_cam, bool isKeyPressed) {
    std::shared_ptr<Program> beeProg = get<PROGRAM>(gameManager[BEE_PROGRAM]);

    std::map<Objs, std::shared_ptr<Shape>> beeShapes =
        get<SHAPES>(gameManager[BEE_PROGRAM]);

    beeProg->bind();
    // send the matrices to the shaders
    glUniformMatrix4fv(beeProg->getUniform("P"), 1, GL_FALSE,
                       value_ptr(Projection->topMatrix()));
    // Always have bee in front of camera
    auto beeV = make_shared<MatrixStack>();
    beeV->pushMatrix();
    beeV->loadIdentity();
    beeV->translate(glm::vec3(0, 0.02, -0.05));
    //        beeV->rotate(glm::half_pi<float>(), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(beeProg->getUniform("V"), 1, GL_FALSE,
                       value_ptr(beeV->topMatrix()));
    // Push, load, add transforms, load, draw, and pop
    Model->pushMatrix();
    Model->loadIdentity();
    Model->translate(glm::vec3(0, 0, 0));

    if (!isKeyPressed)
        Model->rotate(glm::quarter_pi<float>(), glm::vec3(1, 0, 0));
    else
        Model->rotate(rotationAngle, rotatingAxis);
    glUniformMatrix4fv(beeProg->getUniform("M"), 1, GL_FALSE,
                       value_ptr(Model->topMatrix()));
    glUniform3fv(beeProg->getUniform("lightPosition"), 1,
                 &mainLightPosition[0]);
    glUniform3fv(beeProg->getUniform("cameraPosition"), 1, &my_cam.getEye()[0]);
    glm::mat4 beeWorldTransform =
        glm::translate(Model->topMatrix(), my_cam.getEye());
    std::shared_ptr<Shape> beeBody = beeShapes[BEE_BODY_OBJ];
    beeBody->applyTransformationToCollisionBuffer(beeWorldTransform, true);
    beeBody->draw(beeProg);
    for (auto const &pair : get<SHAPES>(gameManager[GROUND_PROGRAM])) {
        std::shared_ptr<Shape> otherShape = (std::shared_ptr<Shape>)pair.second;
        if (beeBody->didCollideWith(otherShape)) {
            DEBUG_MODE && (cout << "YES collision with "
                                << objNames[(Objs)pair.first] << endl);
        } else {
            DEBUG_MODE && (cout << "NO collision with "
                                << objNames[(Objs)pair.first] << endl);
        }
    }
    Model->popMatrix();
    beeV->popMatrix();
    beeProg->unbind();

    return beeProg;
}

std::shared_ptr<Program>
renderBeeWings(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
               shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
               glm::vec3 mainLightPosition, int width,
               std::map<int, Animation> wing_animations, Camera my_cam,
               bool isKeyPressed) {
    std::shared_ptr<Program> beeWingProg =
        get<PROGRAM>(gameManager[BEE_WINGS_PROGRAM]);
    std::map<Objs, std::shared_ptr<Shape>> beeWingShapes =
        get<SHAPES>(gameManager[BEE_WINGS_PROGRAM]);
    static unsigned int kf = 0;

    beeWingProg->bind();
    // send the matrices to the shaders
    glUniformMatrix4fv(beeWingProg->getUniform("P"), 1, GL_FALSE,
                       value_ptr(Projection->topMatrix()));
    // Always have bee in front of camera
    auto beeV = make_shared<MatrixStack>();

    beeV->pushMatrix();
    beeV->loadIdentity();
    beeV->translate(glm::vec3(0, 0.02, -0.05));
    //        beeV->rotate(glm::half_pi<float>(), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(beeWingProg->getUniform("V"), 1, GL_FALSE,
                       value_ptr(beeV->topMatrix()));
    // Push, load, add transforms, load, draw, and pop
    Model->pushMatrix();
    Model->loadIdentity();
    if (isKeyPressed)
        Model->rotate(rotationAngle, rotatingAxis);
    // Animate wings
    Model->translate(
        wing_animations[(kf++ % wing_animations.size())].getTrans());
    Model->rotate(
        wing_animations[(kf++ % wing_animations.size())].getAngle(),
        wing_animations[(kf++ % wing_animations.size())].getRotateAxis());

    glUniformMatrix4fv(beeWingProg->getUniform("M"), 1, GL_FALSE,
                       value_ptr(Model->topMatrix()));
    glUniform3fv(beeWingProg->getUniform("lightPosition"), 1,
                 &mainLightPosition[0]);
    glUniform3fv(beeWingProg->getUniform("cameraPosition"), 1,
                 &my_cam.getEye()[0]);
    glUniform1f(beeWingProg->getUniform("sigma"), 3);
    glUniform1f(beeWingProg->getUniform("blurSize"), 1.0 / width);

    std::shared_ptr<Shape> beeWings = beeWingShapes[BEE_WINGS_OBJ];
    beeWings->draw(beeWingProg);
    Model->popMatrix();
    beeV->popMatrix();
    beeWingProg->unbind();

    return beeWingProg;
}

std::shared_ptr<Program>
renderGround(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
             shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
             glm::vec3 mainLightPosition, Camera my_cam) {
    std::shared_ptr<Program> groundProg =
        get<PROGRAM>(gameManager[GROUND_PROGRAM]);
    std::map<Objs, std::shared_ptr<Shape>> groundShapes =
        get<SHAPES>(gameManager[GROUND_PROGRAM]);
    groundProg->bind();

    setPV(groundProg, Projection, View);

    Model->pushMatrix();
    Model->loadIdentity();
    Model->translate(glm::vec3(0, 0, 0));
    Model->rotate(M_PI_2, glm::vec3(1, 0, 0));
    Model->scale(glm::vec3(40.5, 40.5, 40.5));

    glUniformMatrix4fv(
        groundProg->getUniform(shaderUniformNames[MODEL_UNIFORM]),
        SINGLE_UNIFORM_COUNT, GL_FALSE, value_ptr(Model->topMatrix()));
    glUniform3fv(
        groundProg->getUniform(shaderUniformNames[LIGHT_POSITION_UNIFORM]),
        SINGLE_UNIFORM_COUNT, &mainLightPosition[0]);
    glUniform3fv(
        groundProg->getUniform(shaderUniformNames[CAMERA_POSITION_UNIFORM]),
        SINGLE_UNIFORM_COUNT, &my_cam.getEye()[0]);

    std::shared_ptr<Shape> ground = groundShapes[GROUND_OBJ];
    ground->draw(groundProg);

    Model->popMatrix();
    groundProg->unbind();

    return groundProg;
}

std::shared_ptr<Program>
renderFence(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
            shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
            glm::vec3 mainLightPosition, Camera my_cam) {
    std::shared_ptr<Program> fenceProg =
        get<PROGRAM>(gameManager[FENCE_PROGRAM]);
    std::map<Objs, std::shared_ptr<Shape>> fenceShapes =
        get<SHAPES>(gameManager[FENCE_PROGRAM]);
    fenceProg->bind();

    setPV(fenceProg, Projection, View);

    glUniform3fv(
        fenceProg->getUniform(shaderUniformNames[LIGHT_POSITION_UNIFORM]),
        SINGLE_UNIFORM_COUNT, &mainLightPosition[0]);
    glUniform3fv(
        fenceProg->getUniform(shaderUniformNames[CAMERA_POSITION_UNIFORM]),
        SINGLE_UNIFORM_COUNT, &my_cam.getEye()[0]);

    std::shared_ptr<Shape> fence = fenceShapes[FENCE_OBJ];

    setModel(fenceProg, Model);
    fence->draw(fenceProg);
    fenceProg->unbind();

    return fenceProg;
}

std::shared_ptr<Program>
renderTrees(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
            shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
            glm::vec3 mainLightPosition, Camera my_cam) {
    std::shared_ptr<Program> treeProg =
        get<PROGRAM>(gameManager[TREES_PROGRAM]);
    std::map<Objs, std::shared_ptr<Shape>> treesShapes =
        get<SHAPES>(gameManager[TREES_PROGRAM]);
    treeProg->bind();

    setPV(treeProg, Projection, View);

    glUniform3fv(treeProg->getUniform("lightPosition"), 1,
                 &mainLightPosition[0]);
    glUniform3fv(treeProg->getUniform("cameraPosition"), 1,
                 &my_cam.getEye()[0]);

    std::shared_ptr<Shape> tree = treesShapes[TREES_OBJ];

    Model->pushMatrix();
    Model->loadIdentity();
    Model->scale(vec3(1.5, 2.7, 1.5));
    rotation2 += ROTATE_SPEED2 * get_last_elapsed_time();
    Model->rotate(rotation2, glm::vec3(0, 1, 0));
    setModel(treeProg, Model);
    tree->draw(treeProg);
    Model->popMatrix();
    treeProg->unbind();

    return treeProg;
}

std::shared_ptr<Program>
renderCrocus(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
             shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
             glm::vec3 mainLightPosition, Camera my_cam) {
    std::shared_ptr<Program> crocusProg =
        get<PROGRAM>(gameManager[CROCUS_PROGRAM]);
    std::map<Objs, std::shared_ptr<Shape>> crocusShapes =
        get<SHAPES>(gameManager[CROCUS_PROGRAM]);
    crocusProg->bind();

    setPV(crocusProg, Projection, View);

    glUniform3fv(crocusProg->getUniform("lightPosition"), 1,
                 &mainLightPosition[0]);
    glUniform3fv(crocusProg->getUniform("cameraPosition"), 1,
                 &my_cam.getEye()[0]);

    std::shared_ptr<Shape> crocus = crocusShapes[CROCUS_OBJ];
    float sp = 7.5;
    float off = -0.5;
    Model->pushMatrix();
    Model->loadIdentity();
    Model->scale(glm::vec3(0.3, 0.3, 0.3));

    Model->rotate(-M_PI_2, glm::vec3(1, 0, 0));
    setModel(crocusProg, Model);
    crocus->draw(crocusProg);
    Model->popMatrix();

    crocusProg->unbind();

    return crocusProg;
}

std::shared_ptr<Program>
renderAnemone(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
              shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
              glm::vec3 mainLightPosition, Camera my_cam) {
    std::shared_ptr<Program> anemoneProg =
        get<PROGRAM>(gameManager[ANEMONE_PROGRAM]);
    std::map<Objs, std::shared_ptr<Shape>> anemoneShapes =
        get<SHAPES>(gameManager[ANEMONE_PROGRAM]);
    anemoneProg->bind();

    setPV(anemoneProg, Projection, View);

    glUniform3fv(anemoneProg->getUniform("lightPosition"), 1,
                 &mainLightPosition[0]);
    glUniform3fv(anemoneProg->getUniform("cameraPosition"), 1,
                 &my_cam.getEye()[0]);

    std::shared_ptr<Shape> anemone = anemoneShapes[ANEMONE_OBJ];

    Model->pushMatrix();
    Model->loadIdentity();
    Model->translate(glm::vec3(5, 0, 5));
    Model->scale(glm::vec3(0.5, 0.5, 0.5));
    Model->rotate(-M_PI_2, glm::vec3(1, 0, 0));
    setModel(anemoneProg, Model);
    anemone->draw(anemoneProg);
    Model->popMatrix();
    anemoneProg->unbind();

    return anemoneProg;
}

std::shared_ptr<Program>
renderDaffodil(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
               shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
               glm::vec3 mainLightPosition, Camera my_cam) {
    std::shared_ptr<Program> daffodilProg =
        get<PROGRAM>(gameManager[DAFFODIL_PROGRAM]);
    std::map<Objs, std::shared_ptr<Shape>> daffodilShapes =
        get<SHAPES>(gameManager[DAFFODIL_PROGRAM]);
    daffodilProg->bind();

    setPV(daffodilProg, Projection, View);

    glUniform3fv(daffodilProg->getUniform("lightPosition"), 1,
                 &mainLightPosition[0]);
    glUniform3fv(daffodilProg->getUniform("cameraPosition"), 1,
                 &my_cam.getEye()[0]);

    std::shared_ptr<Shape> daffodil = daffodilShapes[DAFFODIL_OBJ];

    Model->pushMatrix();
    Model->loadIdentity();
    Model->translate(glm::vec3(7, 0, 4));
    Model->scale(glm::vec3(0.5, 0.5, 0.5));
    Model->rotate(-M_PI_2, glm::vec3(1, 0, 0));
    setModel(daffodilProg, Model);
    daffodil->draw(daffodilProg);
    Model->popMatrix();
    daffodilProg->unbind();

    return daffodilProg;
}

std::shared_ptr<Program>
renderWell(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
           shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
           glm::vec3 mainLightPosition, Camera my_cam) {
    std::shared_ptr<Program> wellProg = get<PROGRAM>(gameManager[WELL_PROGRAM]);
    std::map<Objs, std::shared_ptr<Shape>> wellShapes =
        get<SHAPES>(gameManager[WELL_PROGRAM]);
    wellProg->bind();

    setPV(wellProg, Projection, View);

    glUniform3fv(wellProg->getUniform("lightPosition"), 1,
                 &mainLightPosition[0]);
    glUniform3fv(wellProg->getUniform("cameraPosition"), 1,
                 &my_cam.getEye()[0]);

    std::shared_ptr<Shape> well = wellShapes[WELL_OBJ];

    Model->pushMatrix();
    Model->loadIdentity();
    Model->translate(glm::vec3(-22, 3, -12));
    Model->scale(glm::vec3(4, 4, 4));

    setModel(wellProg, Model);
    well->draw(wellProg);
    Model->popMatrix();
    wellProg->unbind();

    return wellProg;
}

std::shared_ptr<Program>
renderPond(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
           shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View,
           glm::vec3 mainLightPosition, Camera my_cam) {
    std::shared_ptr<Program> pondProg = get<PROGRAM>(gameManager[POND_PROGRAM]);
    std::map<Objs, std::shared_ptr<Shape>> pondShapes =
        get<SHAPES>(gameManager[POND_PROGRAM]);
    pondProg->bind();

    setPV(pondProg, Projection, View);

    glUniform3fv(pondProg->getUniform("lightPosition"), 1,
                 &mainLightPosition[0]);
    glUniform3fv(pondProg->getUniform("cameraPosition"), 1,
                 &my_cam.getEye()[0]);

    std::shared_ptr<Shape> pond = pondShapes[POND_OBJ];

    Model->pushMatrix();
    Model->loadIdentity();
    Model->translate(glm::vec3(6, .1, -1));
    Model->scale(glm::vec3(5, 1, 5));
    setModel(pondProg, Model);
    pond->draw(pondProg);
    Model->popMatrix();
    pondProg->unbind();
    return pondProg;
}

std::shared_ptr<Program> renderTwBerries(GAME_BASE_MAP gameManager,
                                         shared_ptr<MatrixStack> Model,
                                         shared_ptr<MatrixStack> Projection,
                                         shared_ptr<MatrixStack> View,
                                         glm::vec3 mainLightPosition,
                                         Camera my_cam) {
    std::shared_ptr<Program> twberriesProg =
        get<PROGRAM>(gameManager[TWBERRIES_PROGRAM]);
    std::map<Objs, std::shared_ptr<Shape>> twberriesShapes =
        get<SHAPES>(gameManager[TWBERRIES_PROGRAM]);
    twberriesProg->bind();

    setPV(twberriesProg, Projection, View);

    glUniform3fv(twberriesProg->getUniform("lightPosition"), 1,
                 &mainLightPosition[0]);
    glUniform3fv(twberriesProg->getUniform("cameraPosition"), 1,
                 &my_cam.getEye()[0]);

    std::shared_ptr<Shape> tberries = twberriesShapes[TWBERRIES_OBJ];

    Model->pushMatrix();
    Model->loadIdentity();
    setModel(twberriesProg, Model);
    tberries->draw(twberriesProg);
    Model->popMatrix();
    twberriesProg->unbind();

    return twberriesProg;
}

std::shared_ptr<Program> renderStonePath(GAME_BASE_MAP gameManager,
                                         shared_ptr<MatrixStack> Model,
                                         shared_ptr<MatrixStack> Projection,
                                         shared_ptr<MatrixStack> View,
                                         glm::vec3 mainLightPosition,
                                         Camera my_cam) {
    std::shared_ptr<Program> stoneProg =
        get<PROGRAM>(gameManager[STONEP_PROGRAM]);
    std::map<Objs, std::shared_ptr<Shape>> stoneShapes =
        get<SHAPES>(gameManager[STONEP_PROGRAM]);
    stoneProg->bind();

    setPV(stoneProg, Projection, View);

    glUniform3fv(stoneProg->getUniform("lightPosition"), 1,
                 &mainLightPosition[0]);
    glUniform3fv(stoneProg->getUniform("cameraPosition"), 1,
                 &my_cam.getEye()[0]);

    std::shared_ptr<Shape> stone = stoneShapes[STONE_OBJ];

    setModel(stoneProg, Model);
    stone->draw(stoneProg);
    stoneProg->unbind();

    return stoneProg;
}

std::shared_ptr<Program>
renderParticles(GAME_BASE_MAP gameManager, shared_ptr<MatrixStack> Model,
                shared_ptr<MatrixStack> Projection,
                shared_ptr<MatrixStack> View, glm::vec3 mainLightPosition,
                particleSys *thePartSystem, Camera my_cam) {
    std::shared_ptr<Program> partProg =
        get<PROGRAM>(gameManager[PARTICLE_PROGRAM]);

    auto particle_view = make_shared<MatrixStack>();

    particle_view->pushMatrix();
    particle_view->loadIdentity();

    Model->pushMatrix();
    Model->loadIdentity();

    partProg->bind();
    Model->translate(vec3(12, 0.5, 8));
    Model->scale(vec3(1, 7, 1));
    setPV(partProg, Projection, View);
    setModel(partProg, Model);

    thePartSystem->drawMe(partProg);
    thePartSystem->update();

    partProg->unbind();

    particle_view->popMatrix();
    Model->popMatrix();

    return partProg;
}
