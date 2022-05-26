#pragma once
#ifndef CAMERA_H_
#define CAMERA_H_

#include "MatrixStack.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <memory>

class Camera {
  public:
    Camera();
    Camera(glm::vec3 eye_in, glm::vec3 lookAt_in, glm::vec3 up_in,
           float speed_in);
    ~Camera();

    /********* EYE **********/
    glm::vec3 getEye();
    void setEye(glm::vec3 new_eye);
    void incrementEye(glm::vec3 add_on);

    /********* LOOK_AT **********/
    glm::vec3 getLookAt();
    void setLookAt(glm::vec3 new_lookAt);
    void incrementLookAt(glm::vec3 add_on);
    glm::vec3 getDeltaLookAt();
    void incrementDeltaLookAt(glm::vec3 add_on);

    /********* UP **********/
    glm::vec3 getUp();
    void setUp(glm::vec3 new_up);

    /********* ROTATE **********/
    float getPitch();
    void setPitch(float new_pitch);
    void incrementPitch(float add_on);
    float getYaw();
    void setYaw(float new_yaw);
    void incrementYaw(float add_on);

    /********* MOVE **********/
    bool getForward();
    void setForward(bool val);

    bool getBackward();
    void setBackward(bool val);

    bool getLeft();
    void setLeft(bool val);

    bool getRight();
    void setRight(bool val);

    bool getThrottle();
    void setThrottle(bool val);

    float getSpeed();

    /********* UPDATE **********/
    void updateCamera(double frame_time, std::shared_ptr<MatrixStack> &V);

  private:
    glm::vec3 eye;
    glm::vec3 lookAt;
    glm::vec3 deltaLookAt;
    glm::vec3 up;

    float speed;
    float pitch;
    float yaw;

    bool forward;
    bool backward;
    bool left;
    bool right;
    bool throttle;

    glm::vec3 acc;
    glm::vec3 vel;
    glm::vec3 pos;
};

#endif
