#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "actor.h"

class Component
{
public:
	// Links the component to an Actor
	Component(Actor* parent);

	// Allows the component to draw itself in the scene
	virtual void draw(mat4 projection, mat4 view) = 0;

	// The owner of this component
	Actor* parent;
};

#endif