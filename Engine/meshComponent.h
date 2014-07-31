#ifndef __MESH_COMPONENT_H__
#define __MESH_COMPONENT_H__

#include "actor.h"
#include "component.h"
#include "globals.h"
#include <vector>

class MeshComponent : public Component
{
public:
	MeshComponent(Actor* parent, const char* modelPath);

	virtual ~MeshComponent();

	// Draws the mesh to the scene
	virtual void draw(mat4 projection, mat4 view);

protected:
	// The raw model data
	std::vector<unsigned short>indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	// Buffers containing model data
	GLuint vertexBuffer;
	GLuint uvBuffer;
	GLuint normalBuffer;
	GLuint elementBuffer;
	GLuint programId;

	// GLSL program references
	GLuint matrixId;
	GLuint viewMatrixId;
	GLuint modelMatrixId;
	GLuint lightId;
	GLuint texture;
	GLuint textureId;
};

#endif