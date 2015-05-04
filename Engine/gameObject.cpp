#include "globals.h"

#include "gameObject.h"
#include "eventManager.h"

namespace HillEngine
{

	GameObject::GameObject()
	{
		globalEventManager.addTickListener(this);
	}

	GameObject::~GameObject()
	{
		globalEventManager.removeTickListener(this);
	}

	void GameObject::tick(float deltaTime)
	{
		// do nothing
	}

} // namespace HillEngine