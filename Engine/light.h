#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "actor.h"
#include "globals.h"

#define LIGHTTYPE_NUM 2
#define LIGHTTYPE_POINT 0
#define LIGHTTYPE_DIRECTIONAL 1

namespace HillEngine
{
	enum LightType
	{
		pointLight,
		directionalLight
	};

	class Light : public Actor
	{
	public:
		Light(unsigned int type, glm::vec3 location, quat rotation, vec3 scale, glm::vec3 col, float pow);

		// Allows the color and power of the light to be changed
		void setColor(glm::vec3 col);
		void setPower(float pow);
		void setLightType(unsigned int newType);

		// Gets the current color and power of the light
		glm::vec3 getColor();
		float getPower();
		unsigned int getLightType();

	private:
		glm::vec3 lightColor;
		float lightPower;
		unsigned int type;
	};

} // namespace HillEngine

#endif