#include <vector>

#include "globals.h"

#include "text2D.h"

#include "loadUtilities.h"

unsigned int text2DTextureId;
unsigned int text2DVertexBufferId;
unsigned int text2DUVBufferId;
unsigned int text2DShaderId;
unsigned int text2DUniformId;

void initText2D(const char* texturePath)
{
	text2DTextureId = loadDDS(texturePath);

	glGenBuffers(1, &text2DVertexBufferId);
	glGenBuffers(1, &text2DUVBufferId);

	text2DShaderId = loadShaders("textVertex.glsl", "textFragment.glsl");

	text2DUniformId = glGetUniformLocation(text2DShaderId, "textureSampler");
}

void printText2D(const char* text, int x, int y, int size)
{
	unsigned int length = strlen(text);

	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> uvs;

	for (unsigned int i = 0; i < length; ++i)
	{
		glm::vec2 vertex_up_left = glm::vec2(x + i * size, y + size);
		glm::vec2 vertex_up_right = glm::vec2(x + i * size + size, y + size);
		glm::vec2 vertex_down_right = glm::vec2(x + i * size + size, y);
		glm::vec2 vertex_down_left = glm::vec2(x + i * size, y);

		vertices.push_back(vertex_up_left);
		vertices.push_back(vertex_down_left);
		vertices.push_back(vertex_up_right);

		vertices.push_back(vertex_down_right);
		vertices.push_back(vertex_up_right);
		vertices.push_back(vertex_down_left);

		char character = text[i];
		float uv_x = (character % 16) / 16.0f;
		float uv_y = (character / 16) / 16.0f;

		glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
		glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 16.0f, uv_y);
		glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / 16.0f, uv_y + 1.0f / 16.0f);
		glm::vec2 uv_down_left = glm::vec2(uv_x, uv_y + 1.0f / 16.0f);

		uvs.push_back(uv_up_left);
		uvs.push_back(uv_down_left);
		uvs.push_back(uv_up_right);

		uvs.push_back(uv_down_right);
		uvs.push_back(uv_up_right);
		uvs.push_back(uv_down_left);
	}

	glBindBuffer(GL_ARRAY_BUFFER, text2DVertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, text2DUVBufferId);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	glUseProgram(text2DShaderId);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, text2DTextureId);

	glUniform1i(text2DUniformId, 0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, text2DVertexBufferId);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, text2DUVBufferId);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void cleanupText2D()
{
	glDeleteBuffers(1, &text2DVertexBufferId);
	glDeleteBuffers(1, &text2DUVBufferId);

	glDeleteTextures(1, &text2DTextureId);

	glDeleteProgram(text2DShaderId);
}