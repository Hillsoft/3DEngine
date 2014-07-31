#include "globals.h"
#include "light.h"
#include "eventManager.h"

Light::Light(glm::vec3 location, quat rotation, vec3 scale, glm::vec3 col, float pow)
	: Actor(location, rotation, scale),
	lightColor(col),
	lightPower(pow)
{
	globalEventManager.addLight(this);
}

void Light::setColor(glm::vec3 col)
{
	lightColor = col;
}

void Light::setPower(float pow)
{
	lightPower = pow;
}

glm::vec3 Light::getColor()
{
	return lightColor;
}

float Light::getPower()
{
	return lightPower;
}
