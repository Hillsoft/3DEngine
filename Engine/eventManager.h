#ifndef __EVENT_MANAGER_H__
#define __EVENT_MANAGER_H__

#include "globals.h"
#include "light.h"

// Actually including these causes circular header referencing, which causes a compiler error, incomplete classes are used instead
class Actor;
class Camera;
class Component;
class GameObject;

// A delegate function used for event response
struct ListenerFunc
{
	ListenerFunc() { function = NULL; owner = NULL; }
	ListenerFunc(void(*f)(int, Actor*), Actor* o) { function = f; owner = o; }

	void(*function)(int, Actor*);
	Actor* owner;
};

// A key binding with a list of listeners
struct Binding
{
	Binding() : name(NULL), key(GLFW_KEY_UNKNOWN), listener(NULL), numListeners(0) { }
	Binding(char* name, int key) : name(name), key(key), listener(NULL), numListeners(0) { }

	int key;
	char* name;
	ListenerFunc* listener;
	int numListeners;
};

class EventManager
{
public:
	EventManager();

	// The main game loop, calls tick and draw functions
	void mainLoop();

	// Functions to deal with key bindings, the names should be self-explanatory
	bool addBinding(const char* name, int key);
	bool addReciever(const char* name, ListenerFunc *listenerFunc);
	void updateBinding(const char* name, int newKey);
	void removeReciever(const char* name, ListenerFunc *listenerFunc);
	bool bindingExists(const char* name);

	// Sets the current camera to be used for drawing
	void setCamera(Camera* newCamera);

	// Functions allowing objects to register for ticking and/or drawing
	void addTickListener(GameObject* obj);
	void addDrawListener(Component* obj);
	void removeTickListener(GameObject* obj);
	void removeDrawListener(Component* obj);

	// Registers a light
	void addLight(Light* light);

private:
	// A static function as C++ function pointers are weird and must be static
	static void staticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	// The non-static version of the above function
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	// Some internal binding helper functions to convert between id and name
	Binding* getBindingWithKey(int key);
	Binding* getBindingWithName(const char* name);

	// The functions that issue a tick and draw call to every registered object
	void tick(float deltaTime);
	void draw(mat4 projection, mat4 view);

	// The list of key bindings
	Binding* keyBindings;
	int numKeyBindings;

	// The current camera
	Camera* currentCamera;

	// The list of objects to tick
	GameObject** tickList;
	int tickNum;
	// The list of objects to draw
	Component** drawList;
	int drawNum;

	// The list of lights
	Light** lightList;
	int lightNum;
};

#endif
