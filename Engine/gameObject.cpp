#include "globals.h"

#include "gameObject.h"
#include "eventManager.h"

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
	
	// do something
}