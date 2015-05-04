#include "globals.h"

#include "component.h"

namespace HillEngine
{

	Component::Component(Actor* parent)
	{
		this->parent = parent;

		parent->components.push_back(this);
	}

} // namespace HillEngine