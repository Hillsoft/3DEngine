#ifndef __POST_PROCESS_H__
#define __POST_PROCESS_H__

namespace HillEngine
{

	// Initializes required post processing variables, FBOs, etc.
	void initPostProcess();
	void cleanupPostProcess();

	// Generates a glow map with only the bright parts of the image
	void genGlowMap(GLuint texOut, GLuint texIn);
	// Blurs the image
	void gaussianBlur(GLuint texOut, GLuint texIn);
	// Causes bright parts of the image to bloom
	void bloom(GLuint& texOut, GLuint texIn);

} // namespace HillEngine

#endif