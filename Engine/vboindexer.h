#ifndef __VBO_INDEXER_H__
#define __VBO_INDEXER_H__

#include <vector>
#include <glm/glm.hpp>

// Indexes model VBO, optimizing draw calls
void indexVBO(
	std::vector<glm::vec3>& in_vertices,
	std::vector<glm::vec2>& in_uvs,
	std::vector<glm::vec3>& in_normals,
	std::vector<unsigned short>& out_indices,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
);

#endif