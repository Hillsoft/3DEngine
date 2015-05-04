#ifndef __SIMPLE_CAMERA_H__
#define __SIMPLE_CAMERA_H__

#include "camera.h"

namespace HillEngine
{

	class SimpleCamera : public Camera
	{
	public:
		SimpleCamera();

		virtual void tick(float deltaTime);

		// Gets the cameras current projection and view matrices
		virtual glm::mat4 getProjectionMatrix();
		virtual glm::mat4 getViewMatrix();

	private:
		// Functions to respond to keyboard input
		static void forwards(int action, Actor* handler);
		static void backwards(int action, Actor* handler);
		static void left(int action, Actor* handler);
		static void right(int action, Actor* handler);
		static void up(int action, Actor* handler);
		static void down(int action, Actor* handler);

		// Keeps track of which keys are currently being held down
		bool keys[6];

		// The projection and view matrices, as calculated in tht tick function
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;

		// Direction of the camera
		float horizontalAngle = 3.14f;
		float verticalAngle = 0.0f;
		// FOV of the camera
		float initialFOV = 45.0f;

		// Mouse control settings
		float speed = 3.0f;
		float mouseSpeed = 0.001f;
	};

} // namespace HillEngine

#endif