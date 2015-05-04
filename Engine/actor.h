#ifndef __ACTOR_H__
#define __ACTOR_H__

#include <glm/gtc/quaternion.hpp>
#include <vector>
#include "gameObject.h"
#include "globals.h"

namespace HillEngine
{

	class Component;

	class Actor : public GameObject
	{
	public:
		Actor();
		Actor(vec3 location, quat rotation, vec3 scale);

		virtual void tick(float deltaTime);

		// The actors position, should maybe be made private...
		vec3 location;
		quat rotation;
		vec3 scale;
		mat4 objectMatrix;

		// The actors components
		std::vector<Component*> components;
	};

} // namespace HillEngine

#endif