#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

class GameObject
{
public:
	GameObject();

	virtual ~GameObject();

	// The tick function, allowing the actor to update itself each frame
	virtual void tick(float deltaTime);
};

#endif