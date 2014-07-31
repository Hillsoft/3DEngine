#ifndef __EVENT_MANAGER_H__
#define __EVENT_MANAGER_H__

#include "globals.h"
#include "light.h"

class Actor;
struct ListenerFunc
{
	ListenerFunc() { function = NULL; owner = NULL; }
	ListenerFunc(void(*f)(int, Actor*), Actor* o) { function = f; owner = o; }

	void(*function)(int, Actor*);
	Actor* owner;
};

struct Binding
{
	Binding() : name(NULL), key(GLFW_KEY_UNKNOWN), listener(NULL), numListeners(0) { }
	Binding(char* name, int key) : name(name), key(key), listener(NULL), numListeners(0) { }

	int key;
	char* name;
	ListenerFunc* listener;
	int numListeners;
};

class Camera;
class Component;
class GameObject;

class EventManager
{
public:
	EventManager();

	void mainLoop();

	bool addBinding(const char* name, int key);
	bool addReciever(const char* name, ListenerFunc *listenerFunc);
	void updateBinding(const char* name, int newKey);
	void removeReciever(const char* name, ListenerFunc *listenerFunc);

	bool bindingExists(const char* name);

	void setCamera(Camera* newCamera);

	void addTickListener(GameObject* obj);
	void addDrawListener(Component* obj);
	void removeTickListener(GameObject* obj);
	void removeDrawListener(Component* obj);

	void addLight(Light* light);

private:
	static void staticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	Binding* getBindingWithKey(int key);
	Binding* getBindingWithName(const char* name);

	void tick(float deltaTime);
	void draw(mat4 projection, mat4 view);

	Binding* keyBindings;
	int numKeyBindings;

	Camera* currentCamera;

	GameObject** tickList;
	int tickNum;
	Component** drawList;
	int drawNum;

	Light** lightList;
	int lightNum;
};

#endif
