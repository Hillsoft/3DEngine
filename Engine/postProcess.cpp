#include "globals.h"

#include "postProcess.h"

#include "loadUtilities.h"

namespace HillEngine
{

	// Frame buffers
	GLuint fboA = 0;
	GLuint fboB = 0;
	GLuint depthRenderBuffer;

	GLuint quad_vertexArray;
	GLuint quad_vertexbuffer;

	// Simple screen covering quad
	static const GLfloat quad_vertexData[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};

	// Glow map variables
	GLuint glowMap;
	GLuint glowMapTex;

	// Gaussian blur variables
	GLuint gaussianHorizontal;
	GLuint gaussianHorizontalTex;
	GLuint gaussianHorizontalPixSize;
	GLuint gaussianVertical;
	GLuint gaussianVerticalTex;
	GLuint gaussianVerticalPixSize;

	// Temporary textures
	GLuint bloomTempTex;
	GLuint blurTempTex;
	GLuint outTex;

	const int bloomWidth = 128;
	const int bloomHeight = 128;

	void initPostProcess()
	{
		// Generate frame buffers
		glGenFramebuffers(1, &fboA);
		glBindFramebuffer(GL_FRAMEBUFFER, fboA);

		glGenFramebuffers(1, &fboB);
		glBindFramebuffer(GL_FRAMEBUFFER, fboB);

		glGenRenderbuffers(1, &depthRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);

		// Initialize quad array
		glGenVertexArrays(1, &quad_vertexArray);
		glBindVertexArray(quad_vertexArray);

		glGenBuffers(1, &quad_vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertexData), quad_vertexData, GL_STATIC_DRAW);

		// Initialize temporary textures
		glGenTextures(1, &bloomTempTex);
		glBindTexture(GL_TEXTURE_2D, bloomTempTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, bloomWidth, bloomHeight, 0, GL_RGB, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glGenTextures(1, &blurTempTex);
		glBindTexture(GL_TEXTURE_2D, blurTempTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, bloomWidth, bloomHeight, 0, GL_RGB, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glGenTextures(1, &outTex);
		glBindTexture(GL_TEXTURE_2D, outTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, bloomWidth, bloomHeight, 0, GL_RGB, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Load shaders
		glowMap = loadShaders("quadVertex.glsl", "glowmapFragment.glsl");
		glowMapTex = glGetUniformLocation(glowMap, "image");

		gaussianHorizontal = loadShaders("quadVertex.glsl", "blurFragmentHorizontal.glsl");
		gaussianHorizontalTex = glGetUniformLocation(gaussianHorizontal, "image");
		gaussianHorizontalPixSize = glGetUniformLocation(gaussianHorizontal, "pixSize");

		gaussianVertical = loadShaders("quadVertex.glsl", "blurFragmentVertical.glsl");
		gaussianVerticalTex = glGetUniformLocation(gaussianVertical, "image");
		gaussianVerticalPixSize = glGetUniformLocation(gaussianVertical, "pixSize");
	}

	void cleanupPostProcess()
	{
		glDeleteTextures(1, &bloomTempTex);

		glDeleteBuffers(1, &quad_vertexbuffer);
	}

	void genGlowMap(GLuint texOut, GLuint texIn)
	{
		// Generate frame buffers
		glBindFramebuffer(GL_FRAMEBUFFER, fboA);
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, bloomWidth, bloomHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texOut, 0);

		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);

		glBindFramebuffer(GL_FRAMEBUFFER, fboA);
		glViewport(0, 0, bloomWidth, bloomHeight);

		// Validate frame buffer
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			printf("Error creating frame buffer\n");

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, bloomWidth, bloomHeight);

		// Initialize glow map textures
		glUseProgram(glowMap);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texIn);
		glUniform1i(glowMapTex, 0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, sizeof(quad_vertexData));

		glDisableVertexAttribArray(0);
	}

	void gaussianBlur(GLuint texOut, GLuint texIn)
	{
		// Bind the frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, fboA);
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, bloomWidth, bloomHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, blurTempTex, 0);

		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);

		glBindFramebuffer(GL_FRAMEBUFFER, fboA);
		glViewport(0, 0, bloomWidth, bloomHeight);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			printf("Error creating frame buffer\n");

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, bloomWidth, bloomHeight);

		// Do the horizontal blur
		glUseProgram(gaussianHorizontal);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texIn);
		glUniform1i(gaussianHorizontalTex, 0);
		glUniform1f(gaussianHorizontalPixSize, 1.0f / bloomHeight);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, sizeof(quad_vertexData));

		glDisableVertexAttribArray(0);


		// TODO: work out why this creates artefacts
		// Switch the frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, fboB);
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, bloomWidth, bloomHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texOut, 0);

		glDrawBuffers(1, drawBuffers);

		glBindFramebuffer(GL_FRAMEBUFFER, fboB);
		glViewport(0, 0, bloomWidth, bloomHeight);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			printf("Error creating frame buffer\n");

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, bloomWidth, bloomHeight);
		
		// Do the vertical blur
		glUseProgram(gaussianVertical);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, blurTempTex);
		glUniform1i(gaussianVerticalTex, 0);
		glUniform1f(gaussianVerticalPixSize, 1.0f / bloomWidth);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, sizeof(quad_vertexData));

		glDisableVertexAttribArray(0);
	}

	void bloom(GLuint& texOut, GLuint texIn)
	{
		if (bloomEnabled)
		{
			// Combine the two required functions
			// genGlowMap(bloomTempTex, texIn);
			gaussianBlur(outTex, texIn);
		}

		texOut = outTex;
	}

} // namespace HillEngine