#ifndef BLOCKDATA_H
#define BLOCKDATA_H

#include <map>
#include <vector>
#include <glm/glm.hpp>

class BlockData
{

public:

	enum class Faces
	{
		FRONT,
		BACK,
		LEFT,
		RIGHT,
		TOP,
		BOTTOM
	};

	enum class BlockType
	{
		GRASS,
		DIRT,
		SAND,
		STONE,
		EMPTY
	};

	struct FaceData
	{
		std::vector<glm::vec3> vertices; // vertex positions
		std::vector<glm::vec2> uvs; // texture coordinates
	};

	static const std::map < Faces, std::vector<glm::vec3>> rawVertexData;
	static const std::map< BlockType, std::tuple<int, int, int, int, int, int>> textureIndices;

	BlockData();

};
#endif