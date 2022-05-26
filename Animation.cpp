#include "Animation.h"
#include <glm/fwd.hpp>

Animation::Animation()
{
	translate = glm::vec3(0, 0, 0);
	rotation_axis = glm::vec3(0, 0, 0);
	rotate_angle = 0;
}

Animation::Animation(glm::vec3 trans, glm::vec3 rotate_axis, float angle)
{
	translate = trans;
	rotation_axis = rotate_axis;
	rotate_angle = angle;

}

Animation::~Animation()
{

}

glm::vec3 Animation::getTrans()
{
	return translate;
}

glm::vec3 Animation::getRotateAxis()
{
	return rotation_axis;
}

float Animation::getAngle()
{
	return rotate_angle;
}
