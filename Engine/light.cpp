#include "globals.h"
#include "light.h"
#include "eventManager.h"

namespace HillEngine
{

	Light::Light(unsigned int type, glm::vec3 location, quat rotation, vec3 scale, glm::vec3 col, float pow)
		: Actor(location, rotation, scale),
		type(type),
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

	void Light::setLightType(unsigned int newType)
	{
		type = newType;
	}

	glm::vec3 Light::getColor()
	{
		return lightColor;
	}

	float Light::getPower()
	{
		return lightPower;
	}

	unsigned int Light::getLightType()
	{
		return type;
	}

}  // namespace HillEngine