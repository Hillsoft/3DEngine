#include <vector>
#include <map>

#include "globals.h"

#include "vboindexer.h"

inline bool is_near(float v1, float v2)
{
	// Defines the limit of when two vertices can be considered the same
	return fabs(v1 - v2) < 0.01f;
}

struct PackedVertex
{
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	// Copy the vertex
	bool operator<(const PackedVertex that) const
	{
		return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0;
	}
};

bool getSimilarVertexIndex(
	PackedVertex& packed,
	std::map<PackedVertex, unsigned short>& vertexToOutIndex,
	unsigned short& result)
{
	// Find a similar enough vertex to merge
	std::map<PackedVertex, unsigned short>::iterator it = vertexToOutIndex.find(packed);

	if (it == vertexToOutIndex.end())
		return false;
	else
	{
		result = it->second;
		return true;
	}
}

void indexVBO(
	std::vector<glm::vec3>& in_vertices,
	std::vector<glm::vec2>& in_uvs,
	std::vector<glm::vec3>& in_normals,
	std::vector<unsigned short>& out_indices,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals)
{
	// Creates a map 
	std::map<PackedVertex, unsigned short> vertexToOutIndex;

	for (unsigned int i = 0; i < in_vertices.size(); ++i)
	{
		PackedVertex packed = { in_vertices[i], in_uvs[i], in_normals[i] };

		// Check if a similar vertex already exists
		unsigned short index;
		bool found = getSimilarVertexIndex(packed, vertexToOutIndex, index);

		if (found)
			out_indices.push_back(index);
		else
		{
			// Add it if it is not found
			out_vertices.push_back(in_vertices[i]);
			out_uvs.push_back(in_uvs[i]);
			out_normals.push_back(in_normals[i]);
			unsigned short newIndex = (unsigned short)out_vertices.size() - 1;
			out_indices.push_back(newIndex);
			vertexToOutIndex[packed] = newIndex;
		}
	}
}