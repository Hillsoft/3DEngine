#ifndef __SIMPLE_CAMERA_H__
#define __SIMPLE_CAMERA_H__

#include "camera.h"

class SimpleCamera : public Camera
{
public:
	SimpleCamera();

	virtual void tick(float deltaTime);

	virtual glm::mat4 getProjectionMatrix();
	virtual glm::mat4 getViewMatrix();
	
private:
	static void forwards(int action, Actor* handler);
	static void backwards(int action, Actor* handler);
	static void left(int action, Actor* handler);
	static void right(int action, Actor* handler);
	static void up(int action, Actor* handler);
	static void down(int action, Actor* handler);

	bool keys[6];

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;

	float horizontalAngle = 3.14f;
	float verticalAngle = 0.0f;
	float initialFOV = 45.0f;

	float speed = 3.0f;
	float mouseSpeed = 0.001f;
};

#endif