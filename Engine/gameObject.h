#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

namespace HillEngine
{

	class GameObject
	{
	public:
		GameObject();

		virtual ~GameObject();

		// The tick function, allowing the actor to update itself each frame
		virtual void tick(float deltaTime);
	};

} // namespace HillEngine

#endif