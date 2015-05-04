#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "globals.h"

#include "actor.h"

#include <stdio.h>
#include <stdlib.h>
#include "eventManager.h"

namespace HillEngine
{

	Actor::Actor()
	{
		location = vec3(0.0f);
		scale = vec3(1.0f);
	}

	Actor::Actor(vec3 location, quat rotation, vec3 scale)
	{
		this->location = location;
		this->rotation = rotation;
		this->scale = scale;
	}

	void Actor::tick(float deltaTime)
	{
		GameObject::tick(deltaTime);

		// Generate the object matrix
		objectMatrix = mat4(1.0f);
		objectMatrix = glm::scale(objectMatrix, scale);
		objectMatrix = toMat4(rotation) * objectMatrix;
		objectMatrix = glm::translate(mat4(1.0f), location) * objectMatrix;
	}

} // namespace HillEngine