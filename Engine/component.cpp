#include "globals.h"

#include "component.h"

Component::Component(Actor* parent)
{
	this->parent = parent;

	parent->components.push_back(this);
}