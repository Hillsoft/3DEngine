#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "actor.h"

namespace HillEngine
{

	class Camera : public Actor
	{
	public:
		// Returns the projection and view matrix generated by the camera
		virtual glm::mat4 getProjectionMatrix() = 0;
		virtual glm::mat4 getViewMatrix() = 0;
	};

}

#endif