#ifndef __POST_PROCESS_H__
#define __POST_PROCESS_H__

void initPostProcess();

void genGlowMap(GLuint texOut, GLuint texIn, int texWidth, int texHeight);
void gaussianBlur(GLuint texOut, GLuint texIn, int texWidth, int texHeight);
void bloom(GLuint texOut, GLuint texIn, int texWidth, int texHeight);

#endif