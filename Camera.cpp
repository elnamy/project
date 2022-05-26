#include "Camera.h"
#include "Helpers.hpp"
#include "MatrixStack.h"
#include <memory>

Camera::Camera() {
    eye = glm::vec3(0, 0, 0);
    lookAt = glm::vec3(0, 0, 0);
    deltaLookAt = glm::vec3(0, 0, 1);
    up = glm::vec3(0, 1, 0);
    speed = 1;

    pitch = 0.0f;
    yaw = -90.0f;

    acc = glm::vec3(0, -0.0098, 0);
    vel = glm::vec3(0, 0, 0);
    pos = glm::vec3(0, 0, 0);

    forward = false;
    backward = false;
    left = false;
    right = false;
    throttle = false;
}

Camera::Camera(glm::vec3 eye_in, glm::vec3 lookAt_in, glm::vec3 up_in,
               float speed_in) {
    eye = eye_in;
    lookAt = lookAt_in;
    deltaLookAt = lookAt_in;
    up = up_in;
    speed = speed_in;

    pitch = 0.0f;
    yaw = -90.0f;

    acc = glm::vec3(0, -0.0098, 0);
    vel = glm::vec3(0, 0, 0);
    pos = glm::vec3(0, 0, 0);

    forward = false;
    backward = false;
    left = false;
    right = false;
    throttle = false;
}

Camera::~Camera() {}

/********* EYE **********/
glm::vec3 Camera::getEye() { return eye; }

void Camera::setEye(glm::vec3 new_eye) { eye = new_eye; }

void Camera::incrementEye(glm::vec3 add_on) { eye += add_on; }

/********* LOOK_AT **********/
glm::vec3 Camera::getLookAt() { return lookAt; }

void Camera::setLookAt(glm::vec3 updated_lookAt) { lookAt = updated_lookAt; }

void Camera::incrementLookAt(glm::vec3 add_on) { lookAt += add_on; }

/********* UP **********/
glm::vec3 Camera::getUp() { return up; }

void Camera::setUp(glm::vec3 new_up) { up = new_up; }

/********* ROTATE **********/
float Camera::getPitch() { return pitch; }

void Camera::setPitch(float new_pitch) { pitch = new_pitch; }

void Camera::incrementPitch(float add_on) { pitch -= add_on; }

float Camera::getYaw() { return yaw; }

void Camera::setYaw(float new_yaw) { yaw = new_yaw; }

void Camera::incrementYaw(float add_on) { yaw += add_on; }

/********* MOVE **********/
bool Camera::getForward() { return forward; }

void Camera::setForward(bool val) { forward = val; }

bool Camera::getBackward() { return backward; }

void Camera::setBackward(bool val) { backward = val; }

bool Camera::getLeft() { return left; }

void Camera::setLeft(bool val) { left = val; }

bool Camera::getRight() { return right; }

void Camera::setRight(bool val) { right = val; }

bool Camera::getThrottle() { return throttle; }

void Camera::setThrottle(bool val) { throttle = val; }

float Camera::getSpeed() { return speed; }

/********* DELTA LOOK AT **********/
glm::vec3 Camera::getDeltaLookAt() { return deltaLookAt; }

void Camera::incrementDeltaLookAt(glm::vec3 add_on) { deltaLookAt += add_on; }

/********* GRAVITY **********/

/********* UPDATE **********/
void Camera::updateCamera(double frame_time, std::shared_ptr<MatrixStack> &V) {
    glm::vec3 direction = glm::vec3(0, 0, 0);

	if (throttle) {
		incrementEye(glm::normalize(up) * (float)frame_time * (speed/2));
	}

    if (forward) {
        incrementEye(glm::vec3((speed * float(frame_time)) * lookAt.x,
                               (speed * float(frame_time)) * lookAt.y +
                                   0.03 * sin(glfwGetTime() * 8),
                               (speed * float(frame_time)) * lookAt.z));
    }

    if (backward) {
        incrementEye(glm::vec3((-speed * float(frame_time)) * lookAt.x,
                               (-speed * float(frame_time)) * lookAt.y +
                                   0.03 * sin(glfwGetTime() * 8),
                               (-speed * float(frame_time)) * lookAt.z));
    }

    if (left) {
        incrementEye(
            glm::vec3((-speed * float(frame_time)) *
                          glm::normalize(glm::cross(lookAt, up)).x,
                      (-speed * float(frame_time)) *
                              glm::normalize(glm::cross(lookAt, up)).y +
                          0.03 * sin(glfwGetTime() * 8),
                      (-speed * float(frame_time)) *
                          glm::normalize(glm::cross(lookAt, up)).z));
    }

    if (right) {
        incrementEye(
            glm::vec3((speed * float(frame_time)) *
                          glm::normalize(glm::cross(lookAt, up)).x,
                      (speed * float(frame_time)) *
                              glm::normalize(glm::cross(lookAt, up)).y +
                          0.03 * sin(glfwGetTime() * 8),
                      (speed * float(frame_time)) *
                          glm::normalize(glm::cross(lookAt, up)).z));
    }

	// idle bounce
	incrementEye(glm::normalize(up) * glm::vec3(0, 0.003 * sin(glfwGetTime() * 8), 0));

	// prevent bee from going through floor
	if (eye.y < 0.1)
		eye.y = 0.1;

	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	lookAt = glm::normalize(direction);
	
	glm::mat4 m_pitch = glm::rotate(glm::mat4(1.0f), glm::radians(pitch), glm::normalize(glm::cross(lookAt, up)));
	up = m_pitch * glm::vec4(0, 1, 0, 1);

    V->lookAt(eye, eye + lookAt, up);
}
