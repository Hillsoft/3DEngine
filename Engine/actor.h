#ifndef __ACTOR_H__
#define __ACTOR_H__

#include <glm/gtc/quaternion.hpp>
#include <vector>
#include "gameObject.h"
#include "globals.h"
class Component;

class Actor : public GameObject
{
public:
	Actor();
	Actor(vec3 location, quat rotation, vec3 scale);

	virtual void tick(float deltaTime);

	vec3 location;
	quat rotation;
	vec3 scale;
	mat4 objectMatrix;

	std::vector<Component*> components;
};

#endif