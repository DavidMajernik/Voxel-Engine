#pragma once
#include "BlockData.h"
#include <glm/vec3.hpp>

class Block
{
public:

	glm::vec3 position;
	BlockData::BlockType type;

	float uMin, vMin, uMax, vMax;
	std::vector<glm::vec2> faceUVs;

	Block();

	Block(glm::vec3 position, BlockData::BlockType type);

	std::vector<glm::vec3> AddTransformedVertices(std::vector<glm::vec3> vertices);

	BlockData::FaceData GetFace(BlockData::Faces face);

	void getUVFromAtlas(int index, int atlasSize, float& uMin, float& vMin, float& uMax, float& vMax);

private:
	std::map<BlockData::Faces, BlockData::FaceData> faces;
};