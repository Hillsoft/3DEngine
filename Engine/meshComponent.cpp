#include "globals.h"

#include "meshComponent.h"

#include "loadUtilities.h"
#include "vboindexer.h"
#include "eventManager.h"

MeshComponent::MeshComponent(Actor* parent, const char* modelPath)
: Component(parent)
{
	// Load the model from the file
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;
	bool res = loadOBJ(modelPath, temp_vertices, temp_uvs, temp_normals);

	// Index VBO for draw call optimization
	indexVBO(temp_vertices, temp_uvs, temp_normals, indices, vertices, uvs, normals);

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	// Load shader
	programId = loadShaders("vertex.glsl", "fragment.glsl");

	matrixId = glGetUniformLocation(programId, "mvp");
	viewMatrixId = glGetUniformLocation(programId, "v");
	modelMatrixId = glGetUniformLocation(programId, "m");

	// Load texture
	texture = loadDDS("Data/uvtemplate.dds");
	textureId = glGetUniformLocation(programId, "textureSampler");

	// Register with event manager
	globalEventManager.addDrawListener(this);
}

MeshComponent::~MeshComponent()
{
	// Clean up
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &elementBuffer);
	glDeleteProgram(programId);
	glDeleteTextures(1, &texture);

	globalEventManager.removeDrawListener(this);
}

void MeshComponent::draw(mat4 projection, mat4 view)
{
	// Generate required matrices
	mat4 model = parent->objectMatrix;
	mat4 mvp = projection * view * model;

	// Draw
	glUseProgram(programId);

	glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(modelMatrixId, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(viewMatrixId, 1, GL_FALSE, &view[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(textureId, 0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}