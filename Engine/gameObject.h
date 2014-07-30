#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

class GameObject
{
public:
	GameObject();

	virtual ~GameObject();

	virtual void tick(float deltaTime);
};

#endif