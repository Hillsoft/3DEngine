#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "actor.h"

class Camera : public Actor
{
public:
	virtual glm::mat4 getProjectionMatrix() = 0;
	virtual glm::mat4 getViewMatrix() = 0;
};

#endif