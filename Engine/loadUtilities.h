#ifndef __LOADUTILITIES_H__
#define __LOADUTILITIES_H__

#include <vector>

GLuint loadShaders(const char* vertexFilePath, const char* fragmentFilePath);

GLuint loadBMP(const char* filePath);
GLuint loadDDS(const char* filePath);

bool loadOBJ(const char* path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs, std::vector<glm::vec3>& out_normals);

#endif