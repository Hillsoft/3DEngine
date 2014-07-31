#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "actor.h"
#include "globals.h"

class Light : public Actor
{
public:
	Light(glm::vec3 location, quat rotation, vec3 scale, glm::vec3 col, float pow);

	void setColor(glm::vec3 col);
	void setPower(float pow);

	glm::vec3 getColor();
	float getPower();

private:
	glm::vec3 lightColor;
	float lightPower;
};

#endif