#include "simpleCamera.h"

#include "globals.h"
#include "eventManager.h"

namespace HillEngine
{

	SimpleCamera::SimpleCamera()
	{
		ListenerFunc* lf;

		// Load key bindings
		if (!globalEventManager.bindingExists("forwards"))
			globalEventManager.addBinding("forwards", GLFW_KEY_W);
		lf = new ListenerFunc(forwards, this);
		globalEventManager.addReciever("forwards", lf);
		delete lf;

		if (!globalEventManager.bindingExists("backwards"))
			globalEventManager.addBinding("backwards", GLFW_KEY_S);
		lf = new ListenerFunc(backwards, this);
		globalEventManager.addReciever("backwards", lf);
		delete lf;

		if (!globalEventManager.bindingExists("left"))
			globalEventManager.addBinding("left", GLFW_KEY_A);
		lf = new ListenerFunc(left, this);
		globalEventManager.addReciever("left", lf);
		delete lf;

		if (!globalEventManager.bindingExists("right"))
			globalEventManager.addBinding("right", GLFW_KEY_D);
		lf = new ListenerFunc(right, this);
		globalEventManager.addReciever("right", lf);
		delete lf;

		if (!globalEventManager.bindingExists("up"))
			globalEventManager.addBinding("up", GLFW_KEY_SPACE);
		lf = new ListenerFunc(up, this);
		globalEventManager.addReciever("up", lf);
		delete lf;

		if (!globalEventManager.bindingExists("down"))
			globalEventManager.addBinding("down", GLFW_KEY_LEFT_SHIFT);
		lf = new ListenerFunc(down, this);
		globalEventManager.addReciever("down", lf);
		delete lf;
	}

	void SimpleCamera::tick(float deltaTime)
	{
		Actor::tick(deltaTime);

		// Get cursor position
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);

		// Rotate based on mouse movement
		horizontalAngle += mouseSpeed * float(windowWidth / 2 - xpos);
		verticalAngle += mouseSpeed * float(windowHeight / 2 - ypos);

		// Calculate forwards vector
		glm::vec3 direction(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle));

		// Calculate right vector
		glm::vec3 right(sin(horizontalAngle - 3.14f / 2.0f), 0, cos(horizontalAngle - 3.14f / 2.0f));

		// Calculate up vector
		glm::vec3 up = glm::cross(right, direction);

		// Move camera if keys are pressed
		if (keys[0])
			location += direction * deltaTime * speed;
		if (keys[1])
			location -= direction * deltaTime * speed;
		if (keys[2])
			location -= right * deltaTime * speed;
		if (keys[3])
			location += right * deltaTime * speed;
		if (keys[4])
			location += up * deltaTime * speed;
		if (keys[5])
			location -= up * deltaTime * speed;

		// Calculate projection and view matrices
		projectionMatrix = glm::perspective(initialFOV, float(windowWidth) / float(windowHeight), 0.1f, 100.0f);
		viewMatrix = glm::lookAt(location, location + direction, up);
	}

	glm::mat4 SimpleCamera::getProjectionMatrix()
	{
		return projectionMatrix;
	}

	glm::mat4 SimpleCamera::getViewMatrix()
	{
		return viewMatrix;
	}

	// Key listeners
	void SimpleCamera::forwards(int action, Actor* handler)
	{
		SimpleCamera* cam = static_cast<SimpleCamera*>(handler);
		if (action == GLFW_PRESS)
			cam->keys[0] = true;
		else
			cam->keys[0] = false;
	}

	void SimpleCamera::backwards(int action, Actor* handler)
	{
		SimpleCamera* cam = static_cast<SimpleCamera*>(handler);
		if (action == GLFW_PRESS)
			cam->keys[1] = true;
		else
			cam->keys[1] = false;
	}

	void SimpleCamera::left(int action, Actor* handler)
	{
		SimpleCamera* cam = static_cast<SimpleCamera*>(handler);
		if (action == GLFW_PRESS)
			cam->keys[2] = true;
		else
			cam->keys[2] = false;
	}

	void SimpleCamera::right(int action, Actor* handler)
	{
		SimpleCamera* cam = static_cast<SimpleCamera*>(handler);
		if (action == GLFW_PRESS)
			cam->keys[3] = true;
		else
			cam->keys[3] = false;
	}

	void SimpleCamera::up(int action, Actor* handler)
	{
		SimpleCamera* cam = static_cast<SimpleCamera*>(handler);
		if (action == GLFW_PRESS)
			cam->keys[4] = true;
		else
			cam->keys[4] = false;
	}

	void SimpleCamera::down(int action, Actor* handler)
	{
		SimpleCamera* cam = static_cast<SimpleCamera*>(handler);
		if (action == GLFW_PRESS)
			cam->keys[5] = true;
		else
			cam->keys[5] = false;
	}

} // namespace HillEngine