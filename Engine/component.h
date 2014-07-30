#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "actor.h"

class Component
{
public:
	Component(Actor* parent);

	virtual void draw(mat4 projection, mat4 view) = 0;

	Actor* parent;
};

#endif