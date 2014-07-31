#ifndef __GLOBALS_H__
#define __GLOABLS_H__

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#ifndef M_PI
// For some reason, this isn't defined in every compiler
#define M_PI 3.14159265
#endif

// Circular header includes are bad
class EventManager;

// Graphics settings
extern int windowWidth;
extern int windowHeight;
extern bool fullscreen;
extern int msaaRes;
// A link to the GL window object
extern GLFWwindow* window;

// Deltatime, still used by performanceMonitor, but otherwise deprecated
extern double deltaTime;

// The global event manager, a very important object
extern EventManager globalEventManager;

// Loads game settings from engine.ini and keyBindings.ini
void loadSettings();

#endif