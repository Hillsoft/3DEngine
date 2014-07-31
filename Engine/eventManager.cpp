#include <stdio.h>
#include <iostream>

#include "globals.h"

#include "eventManager.h"

#include "actor.h"
#include "camera.h"
#include "component.h"
#include "loadUtilities.h"
#include "performanceMonitor.h"
#include "postProcess.h"

EventManager::EventManager() : keyBindings(NULL), numKeyBindings(0)
{
	// Set some default values
	tickList = new GameObject*[0];
	drawList = new Component*[0];
	tickNum = 0;
	drawNum = 0;
}

void EventManager::mainLoop()
{
	// Assigns this object as the key handler for the application
	glfwSetKeyCallback(window, staticKeyCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// A simple quad which covers the entire screen, has many uses
	GLuint quad_vertexArray;
	glGenVertexArrays(1, &quad_vertexArray);
	glBindVertexArray(quad_vertexArray);

	static const GLfloat quad_vertexData[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};

	GLuint quad_vertexbuffer;
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertexData), quad_vertexData, GL_STATIC_DRAW);

	// Loads post processing shaders
	GLuint ppProgramId = loadShaders("quadVertex.glsl", "ppFragment.glsl");
	GLuint texId = glGetUniformLocation(ppProgramId, "renderOutput");
	GLuint bloomId = glGetUniformLocation(ppProgramId, "bloom");

	// Loads deferred shading shaders
	GLuint deferredProgramId = loadShaders("quadVertex.glsl", "deferredFragment.glsl");
	GLuint positionTexId = glGetUniformLocation(deferredProgramId, "position_cameraspaceTex");
	GLuint diffuseTexId = glGetUniformLocation(deferredProgramId, "diffuseTex");
	GLuint specularTexId = glGetUniformLocation(deferredProgramId, "specularTex");
	GLuint normalTexId = glGetUniformLocation(deferredProgramId, "normalTex");
	GLuint lightPositionId = glGetUniformLocation(deferredProgramId, "lightPosition_cameraspace");
	GLuint lightColorId = glGetUniformLocation(deferredProgramId, "lightColor");
	GLuint lightPowerId = glGetUniformLocation(deferredProgramId, "lightPower");

	// Initializes the main draw frame buffer
	GLuint frameBufferName = 0;
	glGenFramebuffers(1, &frameBufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferName);

	// Intialize a lot of output textures for deferred shading
	GLuint positionOutput;
	glGenTextures(1, &positionOutput);
	glBindTexture(GL_TEXTURE_2D, positionOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLuint diffuseOutput;
	glGenTextures(1, &diffuseOutput);
	glBindTexture(GL_TEXTURE_2D, diffuseOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLuint specularOutput;
	glGenTextures(1, &specularOutput);
	glBindTexture(GL_TEXTURE_2D, specularOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLuint normalOutput;
	glGenTextures(1, &normalOutput);
	glBindTexture(GL_TEXTURE_2D, normalOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLuint colorOutput;
	glGenTextures(1, &colorOutput);
	glBindTexture(GL_TEXTURE_2D, colorOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Initializes some post processing textures
	GLuint glowMapTex;
	glGenTextures(1, &glowMapTex);
	glBindTexture(GL_TEXTURE_2D, glowMapTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 128, 128, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Intializes the depth render buffer, required for render to texture
	GLuint depthRenderBuffer;
	glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

	// Assigns textures to the main draw frame buffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, positionOutput, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, diffuseOutput, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, specularOutput, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, normalOutput, 0);

	// Check the validity of the frame buffer
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("Error creating frame buffer");

	// Initialize the defferd shading frame buffer and assign its output texture
	GLuint deferredFrameBuffer = 0;
	glGenFramebuffers(1, &deferredFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, deferredFrameBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorOutput, 0);

	// Check the frame buffers validity
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("Error creating frame buffer");

	// Used to calculate delta time
	double lastTime = glfwGetTime();

	// The main game loop
	bool cont = true;
	while (cont)
	{
		// Calculates the time since the previous frame
		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		// Check if the escape key is pressed, if so, exit
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			cont = false;

		// Send the tick call to every game object
		tick(float(deltaTime));

		// Activate the main draw framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferName);
		glViewport(0, 0, windowWidth, windowHeight);

		GLenum drawBuffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, drawBuffers);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		// Get view and projection matrices for the camera
		glm::mat4 projectionMatrix = currentCamera->getProjectionMatrix();
		glm::mat4 viewMatrix = currentCamera->getViewMatrix();

		// Draw every object
		draw(projectionMatrix, viewMatrix);

		// Deferred shading, initialize non light-related variables
		glBindFramebuffer(GL_FRAMEBUFFER, deferredFrameBuffer);
		glViewport(0, 0, windowWidth, windowHeight);

		GLenum drawBuffersDeferred[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffersDeferred);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		// Used to enable additive blending between lights
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glUseProgram(deferredProgramId);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, positionOutput);
		glUniform1i(positionTexId, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, diffuseOutput);
		glUniform1i(diffuseTexId, 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, specularOutput);
		glUniform1i(specularTexId, 2);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalOutput);
		glUniform1i(normalTexId, 3);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Render for each light
		for (int i = 0; i < lightNum; ++i)
		{
			glm::vec4 lightPosition_cameraspace = viewMatrix * vec4(lightList[i]->location, 1);
			glUniform3f(lightPositionId, lightPosition_cameraspace.x, lightPosition_cameraspace.y, lightPosition_cameraspace.z);
			glUniform3f(lightColorId, lightList[i]->getColor().r, lightList[i]->getColor().g, lightList[i]->getColor().b);
			glUniform1f(lightPowerId, lightList[i]->getPower());

			glDrawArrays(GL_TRIANGLES, 0, sizeof(quad_vertexData) / sizeof(GLfloat));
		}

		glDisableVertexAttribArray(0);

		// Does all of the bloom, currently disabled as something is wrong with it, even though it used to work...
		// bloom(glowMapTex, colorOutput, 128, 128);

		// Binds the screen as the frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, windowWidth, windowHeight);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Combines the post processing output with the raw output
		glUseProgram(ppProgramId);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorOutput);
		glUniform1i(texId, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, glowMapTex);
		glUniform1i(bloomId, 1);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		
		glDrawArrays(GL_TRIANGLES, 0, sizeof(quad_vertexData) / sizeof(GLfloat));

		glDisableVertexAttribArray(0);

#ifdef _DEBUG
		drawPerformanceGraphs();
#endif

		// Swap buffers to prevent screen tearing
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up data
	glDeleteBuffers(1, &quad_vertexbuffer);
	glDeleteBuffers(1, &frameBufferName);
	glDeleteBuffers(1, &depthRenderBuffer);
	glDeleteProgram(ppProgramId);
	glDeleteProgram(deferredProgramId);
	glDeleteTextures(1, &positionOutput);
	glDeleteTextures(1, &diffuseOutput);
	glDeleteTextures(1, &specularOutput);
	glDeleteTextures(1, &normalOutput);
	glDeleteTextures(1, &colorOutput);
	glDeleteTextures(1, &glowMapTex);
	glDeleteVertexArrays(1, &quad_vertexArray);

	glfwTerminate();
}

void EventManager::addTickListener(GameObject* obj)
{
	// Create new array by copying old array and appending new item
	GameObject** newTick = new GameObject*[tickNum + 1];
	memcpy(newTick, tickList, sizeof(GameObject*)* tickNum);
	newTick[tickNum] = obj;

	// Deletes the old array and assigns the new one as the variable
	delete[] tickList;
	tickList = newTick;

	tickNum++;
}

void EventManager::addDrawListener(Component* obj)
{
	// Create new array by copying old array and appending new item
	Component** newDraw = new Component*[drawNum + 1];
	memcpy(newDraw, drawList, sizeof(Component*) * drawNum);
	newDraw[drawNum] = obj;

	// Deletes the old array and assigns the new one as the variable
	delete[] drawList;
	drawList = newDraw;

	drawNum++;
}

void EventManager::removeTickListener(GameObject* obj)
{
	GameObject** newTick = new GameObject*[tickNum - 1];

	for (int i = 0; i <= tickNum; ++i)
	{
		// Find the object being removed
		if (tickList[i] == obj)
		{
			// Copy the front and back sections of the old array into the new array, omitting the item that is to be removed
			memcpy(newTick, tickList, sizeof(GameObject*)* i);
			memcpy(&(newTick[i]), tickList[i + 1], sizeof(GameObject*)* (tickNum - i));

			delete[] tickList;
			tickList = newTick;

			tickNum--;
			return;
		}
	}
}

void EventManager::removeDrawListener(Component* obj)
{
	Component** newDraw = new Component*[drawNum - 1];

	for (int i = 0; i <= drawNum; ++i)
	{
		// Find the object being removed
		if (drawList[i] == obj)
		{
			// Copy the front and back sections of the old array into the new array, omitting the item that is to be removed
			memcpy(newDraw, drawList, sizeof(Component*)* i);
			memcpy(&(newDraw[i]), drawList[i + 1], sizeof(Component*)* (tickNum - i));

			delete[] drawList;
			drawList = newDraw;

			drawNum--;
			return;
		}
	}
}

void EventManager::tick(float deltaTime)
{
	// Send tick message to every item in the tickList
	for (int i = 0; i < tickNum; ++i)
		tickList[i]->tick(deltaTime);
}

void EventManager::draw(mat4 projection, mat4 view)
{
	// Send draw message to every item in the drawList
	for (int i = 0; i < drawNum; ++i)
		drawList[i]->draw(projection, view);
}

bool EventManager::addBinding(const char* name, int key)
{
	// Cannot add the binding if it already exists
	if (getBindingWithName(name) != NULL)
		return false;

	if (numKeyBindings == 0)
	{
		// keyBindings array will be uninitialized, initialize it and enter data
		keyBindings = new Binding[1];
		keyBindings[0].name = new char[strlen(name) + 1];
		memcpy(keyBindings[0].name, name, strlen(name) + 1);
		keyBindings[0].key = key;
		keyBindings[0].listener = NULL;
		keyBindings[0].numListeners = 0;
		numKeyBindings++;
	}
	else
	{
		// Creates a new, extended key bindings array
		numKeyBindings++;
		Binding* newBindings = new Binding[numKeyBindings];

		// The for loop populates the data one item at a time - the array is sorted by the key id
		bool addedNewBinding = false;
		int oldIndex = 0;
		for (int i = 0; i < numKeyBindings; ++i)
		{
			if (addedNewBinding || (keyBindings[oldIndex].key < key && oldIndex < numKeyBindings - 1))
			{
				// The new item is either already added or is not yet ready to be added
				newBindings[i] = keyBindings[oldIndex];
				oldIndex++;
			}
			else
			{
				// Insert the new item
				newBindings[i].name = new char[strlen(name) + 1];
				memcpy(newBindings[i].name, name, strlen(name) + 1);
				newBindings[i].key = key;
				newBindings[i].listener = NULL;
				newBindings[i].numListeners = 0;
				addedNewBinding = true;
			}
		}

		delete[] keyBindings;
		keyBindings = newBindings;
	}

	return true;
}

bool EventManager::addReciever(const char* name, ListenerFunc *listenerFunc)
{
	// Gets the binding the reciever is to be added to
	Binding* b = getBindingWithName(name);

	// If the binding does not exist, the reciever cannot be added
	if (b == NULL)
		return false;

	// Create new listener array, copy old data and append new item
	ListenerFunc* newListeners = new ListenerFunc[b->numListeners + 1];

	memcpy(newListeners, b->listener, sizeof(ListenerFunc) * b->numListeners);
	delete[] b->listener;

	newListeners[b->numListeners] = *listenerFunc;
	b->numListeners++;

	b->listener = newListeners;

	return true;
}

void EventManager::updateBinding(const char* name, int newKey)
{
	// Gets the biding to be updated
	Binding *b = getBindingWithName(name);

	if (b == NULL)
	{
		// Add the binding if it does not exist
		addBinding(name, newKey);
	}
	else
	{
		// Update the bindings key
		b->key = newKey;
	}
}

void EventManager::removeReciever(const char* name, ListenerFunc *listenerFunc)
{
	// Gets the binding which the listener is to be removed from
	Binding* b = getBindingWithName(name);

	// Cannot remove the listener if the reciever does not exist
	if (b == NULL)
		return;

	// Creates the new listener array
	ListenerFunc* newListeners = new ListenerFunc[b->numListeners - 1];

	int i = 0;
	int ni = 0;
	// Adds each old item one at a time, omitting it if it is the one to be removed
	while (i < b->numListeners)
	{
		if (b->listener[i].owner != listenerFunc->owner || b->listener[i].function != listenerFunc->function)
		{
			newListeners[ni] = b->listener[i];
			ni++;
		}

		i++;
	}

	delete[] b->listener;

	b->listener = newListeners;
}

bool EventManager::bindingExists(const char* name)
{
	if (getBindingWithName(name) == NULL)
		return false;

	return true;
}

void EventManager::staticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	globalEventManager.keyCallback(window, key, scancode, action, mods);
}

void EventManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS && action != GLFW_RELEASE)
		return;

	Binding* binding = getBindingWithKey(key);

	// Exit if there is no associated binding
	if (binding == NULL)
		return;

	// Call each function in the bindings listener array
	for (int i = 0; i < binding->numListeners; ++i)
	{
		(binding->listener[i].function)(action, binding->listener[i].owner);
	}
}

Binding* EventManager::getBindingWithKey(int key)
{
	if (numKeyBindings == 0)
		return NULL;

	// Uses a binary search algorithm to quickly find the correct binding
	int lo = 0;
	int hi = numKeyBindings - 1;
	while (lo < hi)
	{
		int mid = (lo + hi) / 2;

		if (mid == hi)
			mid--;

		if (keyBindings[mid].key < key)
			lo = mid + 1;
		else
			hi = mid;
	}

	if (lo == hi && keyBindings[lo].key == key)
		return &(keyBindings[lo]);
	else
		return NULL;
}

Binding* EventManager::getBindingWithName(const char* name)
{
	for (int i = 0; i < numKeyBindings; ++i)
	{
		if (!strcmp(name, keyBindings[i].name))
		{
			return &(keyBindings[i]);
		}
	}

	return NULL;
}

void EventManager::setCamera(Camera* newCamera)
{
	currentCamera = newCamera;
}

void EventManager::addLight(Light* newLight)
{
	// Adds a new light
	Light** newLightList = new Light*[lightNum + 1];
	memcpy(newLightList, lightList, sizeof(Light*)* lightNum);
	newLightList[lightNum] = newLight;

	delete[] lightList;
	lightList = newLightList;

	lightNum++;
}
