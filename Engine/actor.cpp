#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "globals.h"

#include "actor.h"

#include <stdio.h>
#include <stdlib.h>
#include "eventManager.h"

Actor::Actor()
{
	location = vec3(0.0f);
	scale = vec3(1.0f);
}

void Actor::tick(float deltaTime)
{
	GameObject::tick(deltaTime);

	vec3 eulerRotation = vec3(0, 1 * deltaTime, 0);
	// rotation = quat(eulerRotation) * rotation;

	objectMatrix = mat4(1.0f);
	objectMatrix = glm::scale(objectMatrix, scale);
	objectMatrix = toMat4(rotation) * objectMatrix;
	objectMatrix = glm::translate(mat4(1.0f), location) * objectMatrix;
}