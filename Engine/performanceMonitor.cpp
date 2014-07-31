#ifdef _DEBUG

#include <stdio.h>

#include "globals.h"

#include "performanceMonitor.h"
#include "text2D.h"

double lastTime = 0;
int nbFrames = 0;
float avgTime = 0;

void drawPerformanceGraphs()
{
	glDisable(GL_DEPTH_TEST);

	double currentTime = glfwGetTime();
	nbFrames++;

	if (currentTime - lastTime > 1.0)
	{
		// Calculate average render time in ms
		avgTime = float(1000 / double(nbFrames));
		nbFrames = 0;
		lastTime = currentTime;
	}

	char text[16];
	sprintf(text, "%2.2fms", avgTime);
	printText2D(text, 0, 0, 20);

	glEnable(GL_DEPTH_TEST);
}

#endif