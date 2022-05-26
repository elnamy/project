#ifndef ANIMATION_H_
#define ANIMATION_H_

#include "glm/glm.hpp"
#include <memory>

class Animation 
{
public:
	Animation();
	Animation(glm::vec3 trans, glm::vec3 rotate_axis, float angle);
	~Animation();

	glm::vec3 getTrans();
	glm::vec3 getRotateAxis();
	float getAngle();


private:
	glm::vec3 translate;
	glm::vec3 rotation_axis;
	float rotate_angle;

};

#endif /*ANIMATION_H_ */
