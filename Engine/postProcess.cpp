#include "globals.h"

#include "postProcess.h"

#include "loadUtilities.h"

GLuint fboA = 0;
GLuint fboB = 0;
GLuint depthRenderBuffer;
GLuint quad_vertexArray;
GLuint quad_vertexbuffer;

static const GLfloat quad_vertexData[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 0.0f
};

GLuint glowMap;
GLuint glowMapTex;

GLuint gaussianHorizontal;
GLuint gaussianHorizontalTex;
GLuint gaussianHorizontalPixSize;
GLuint gaussianVertical;
GLuint gaussianVerticalTex;
GLuint gaussianVerticalPixSize;

void initPostProcess()
{
	glGenFramebuffers(1, &fboA);
	glBindFramebuffer(GL_FRAMEBUFFER, fboA);

	glGenFramebuffers(1, &fboB);
	glBindFramebuffer(GL_FRAMEBUFFER, fboB);

	glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);

	glGenVertexArrays(1, &quad_vertexArray);
	glBindVertexArray(quad_vertexArray);

	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertexData), quad_vertexData, GL_STATIC_DRAW);

	glowMap = loadShaders("quadVertex.glsl", "glowmapFragment.glsl");
	glowMapTex = glGetUniformLocation(glowMap, "image");

	gaussianHorizontal = loadShaders("quadVertex.glsl", "blurFragmentHorizontal.glsl");
	gaussianHorizontalTex = glGetUniformLocation(gaussianHorizontal, "image");
	gaussianHorizontalPixSize = glGetUniformLocation(gaussianHorizontal, "pixSize");

	gaussianVertical = loadShaders("quadVertex.glsl", "blurFragmentVertical.glsl");
	gaussianVerticalTex = glGetUniformLocation(gaussianVertical, "image");
	gaussianVerticalPixSize = glGetUniformLocation(gaussianVertical, "pixSize");
}

void genGlowMap(GLuint texOut, GLuint texIn, int texWidth, int texHeight)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fboA);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texWidth, texHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texOut, 0);

	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	glBindFramebuffer(GL_FRAMEBUFFER, fboA);
	glViewport(0, 0, texWidth, texHeight);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("Error creating frame buffer\n");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, texWidth, texHeight);

	glUseProgram(glowMap);

	glActiveTexture(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texIn);
	glUniform1i(glowMapTex, 0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, sizeof(quad_vertexData));

	glDisableVertexAttribArray(0);
}

void gaussianBlur(GLuint texOut, GLuint texIn, int texWidth, int texHeight)
{
	GLuint tempTex;
	glGenTextures(1, &tempTex);
	glBindTexture(GL_TEXTURE_2D, tempTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, texWidth, texHeight, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, fboA);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texWidth, texHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tempTex, 0);

	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	glBindFramebuffer(GL_FRAMEBUFFER, fboA);
	glViewport(0, 0, texWidth, texHeight);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("Error creating frame buffer\n");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, texWidth, texHeight);

	glUseProgram(gaussianHorizontal);
	
	glActiveTexture(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texIn);
	glUniform1i(gaussianHorizontalTex, 0);
	glUniform1f(gaussianHorizontalPixSize, 1.0f / texHeight);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, sizeof(quad_vertexData));

	glDisableVertexAttribArray(0);


	glBindFramebuffer(GL_FRAMEBUFFER, fboB);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texWidth, texHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texOut, 0);

	glDrawBuffers(1, drawBuffers);

	glBindFramebuffer(GL_FRAMEBUFFER, fboB);
	glViewport(0, 0, texWidth, texHeight);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("Error creating frame buffer\n");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, texWidth, texHeight);

	glUseProgram(gaussianVertical);

	glActiveTexture(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tempTex);
	glUniform1i(gaussianVerticalTex, 0);
	glUniform1f(gaussianVerticalPixSize, 1.0f / texWidth);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, sizeof(quad_vertexData));

	glDisableVertexAttribArray(0);

	glDeleteTextures(1, &tempTex);
}

void bloom(GLuint texOut, GLuint texIn, int texWidth, int texHeight)
{
	GLuint tempTex;
	glGenTextures(1, &tempTex);
	glBindTexture(GL_TEXTURE_2D, tempTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, texWidth, texHeight, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	genGlowMap(tempTex, texIn, texWidth, texHeight);
	gaussianBlur(texOut, tempTex, texWidth, texHeight);

	glDeleteTextures(1, &tempTex);
}