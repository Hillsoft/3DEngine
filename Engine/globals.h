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
#define M_PI 3.14159265
#endif

class GenericClass;
class EventManager;

extern int windowWidth;
extern int windowHeight;
extern bool fullscreen;
extern int msaaRes;
extern GLFWwindow* window;

extern double deltaTime;

extern EventManager globalEventManager;

void loadSettings();

#endif