#ifndef __TEXT2D_H__
#define __TEXT2D_H__

namespace HillEngine
{

	// Initializes required variables, textures, etc.
	void initText2D(const char* texturePath);
	// Prints some text to the screen
	void printText2D(const char* text, int x, int y, int size);
	// Disposes of variables, textures, etc.
	void cleanupText2D();

} // namespace HillEngine

#endif