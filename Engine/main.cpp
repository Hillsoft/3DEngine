#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "globals.h"

#include "actor.h"
#include "eventManager.h"
#include "light.h"
#include "loadUtilities.h"
#include "meshComponent.h"
#include "performanceMonitor.h"
#include "postProcess.h"
#include "simpleCamera.h"
#include "text2D.h"
#include "vboindexer.h"

using namespace HillEngine;

int main()
{
	// Loads engine settings before initialization
	loadSettings();

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialise GLFW.\n");
		return -1;
	}
	
	glfwWindowHint(GLFW_SAMPLES, msaaRes);
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 3);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create game window
	window = glfwCreateWindow(windowWidth, windowHeight, "Test", fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialise GLEW.\n");
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);


	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Initialize test scene
	initText2D("Data/Holstein.dds");
	initPostProcess();

	Actor* suzanne = new Actor();
	MeshComponent* suzanneMesh = new MeshComponent(suzanne, "Data/suzanne.obj");

	Actor* suzanne2 = new Actor();
	MeshComponent* suzanneMesh2 = new MeshComponent(suzanne2, "Data/suzanne.obj");
	suzanne2->location = vec3(-2.0f, 0.0f, -2.0f);
	suzanne2->rotation = quat(vec3(0, -M_PI / 2, 0));

	Actor* suzanne3 = new Actor();
	MeshComponent* suzanneMesh3 = new MeshComponent(suzanne3, "Data/suzanne.obj");
	suzanne3->location = vec3(2.0f, 0.0f, -2.0f);
	suzanne3->rotation = quat(vec3(0, M_PI / 2, 0));

	Actor* suzanne4 = new Actor();
	MeshComponent* suzanneMesh4 = new MeshComponent(suzanne4, "Data/suzanne.obj");
	suzanne4->location = vec3(0.0f, 0.0f, -4.0f);
	suzanne4->rotation = quat(vec3(0, M_PI, 0));

	Actor* plane = new Actor();
	MeshComponent* planeMesh = new MeshComponent(plane, "Data/plane.obj");
	plane->location = vec3(0.0f, -1.0f, 0.0f);
	plane->scale = vec3(100.0f, 1.0f, 100.0f);

	SimpleCamera* cam = new SimpleCamera();
	globalEventManager.setCamera(cam);

	Light* redLight = new Light(LIGHTTYPE_POINT ,vec3(0, 7, 0), quat(), vec3(1), vec3(1, 0, 0), 20);
	Light* blueLight = new Light(LIGHTTYPE_POINT, vec3(0, 7, -4), quat(), vec3(1), vec3(0, 0, 1), 20);
	Light* greenLight = new Light(LIGHTTYPE_POINT, vec3(0, 0, -2), quat(), vec3(1), vec3(0, 1, 0), 1);
	Light* brightLight = new Light(LIGHTTYPE_POINT, vec3(0, 7, -2), quat(), vec3(1), vec3(1, 1, 1), 30);
	// Light* sunLight = new Light(LIGHTTYPE_DIRECTIONAL, vec3(0, 0, 0), quat(), vec3(1), vec3(1, 1, 1), 1);

	globalEventManager.mainLoop();
}