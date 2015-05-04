#ifndef __LOADUTILITIES_H__
#define __LOADUTILITIES_H__

#include "globals.h"

#include <vector>
#include <al/al.h>
#include <al/alc.h>

namespace HillEngine
{

	// Loads glsl shaders from a file, returns the id
	GLuint loadShaders(const char* vertexFilePath, const char* fragmentFilePath);

	// Loads textures from a file (DDS is preferable and required for mipmapping)
	GLuint loadBMP(const char* filePath);
	GLuint loadDDS(const char* filePath);

	// Loads objects from a file
	bool loadOBJ(const char* path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs, std::vector<glm::vec3>& out_normals);

	// Loads a WAV from a file
	bool loadWAVFile(const char* filename, ALuint* buffer, ALsizei* size, ALsizei* frequency, ALenum* format);

} // namespace HillEngine

#endif