#ifndef __POST_PROCESS_H__
#define __POST_PROCESS_H__

// Initializes required post processing variables, FBOs, etc.
void initPostProcess();

// Generates a glow map with only the bright parts of the image
void genGlowMap(GLuint texOut, GLuint texIn, int texWidth, int texHeight);
// Blurs the image
void gaussianBlur(GLuint texOut, GLuint texIn, int texWidth, int texHeight);
// Causes bright parts of the image to bloom
void bloom(GLuint texOut, GLuint texIn, int texWidth, int texHeight);

#endif