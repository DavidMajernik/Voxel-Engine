#pragma once

#include "World_Constants.h"
#include <unordered_map>
#include <glm/gtx/hash.hpp>
#include <array>
#include <glm/glm.hpp> 
#include "../Texture.h"
#include "../Shader.h"
#include <iostream>
#include <chrono>
#include "Terrain.h"
#include "Mesh.h"
#include <random>

using BlockPosition = glm::vec3; // Using glm's ivec3 for block positions

class Chunk {

	struct ChunkData {
		std::array<uint8_t, chunkVolume> blocks; // Stores block data for the chunk

		uint8_t getBlock(const BlockPosition& blockPos) const;
		void setBlock(const BlockPosition& blockPos, uint8_t blockType);

	};

public:

	bool isDirty = false; // Indicates if the chunk needs to be regenerated

	Chunk();
	Chunk(glm::vec3 pos);

	glm::vec3 chunkPos;

	static Texture* texture;
	static std::array<std::array<std::array<float, 4>, 6>, 256> cachedUVs;
	std::array<std::array<int, (chunkSize + padding)>, (chunkSize + padding)> heightMap;

	static void initializeTexture();
	static void cleanupTexture();
	static void cacheUVsFromAtlas();

	void genBlocks(std::array<std::array<int, (chunkSize + padding)>, (chunkSize + padding)> &heightMap);
	void genFeatures(std::array<std::array<int, (chunkSize + padding)>, (chunkSize + padding)>& heightMap);
	void genFaces();
	void integrateFace(BlockPosition blockPos, Faces face);
	void addIndices(int amtFaces, bool water);
	void uploadToGPU();
	void renderSolids(Shader& shader);
	void renderWater(Shader& waterShader);
	void Delete();

	void setBlock(const BlockPosition& blockPos, uint8_t blockType);
	uint8_t getBlock(const BlockPosition& blockPos) const;

	void generateAOVals(BlockPosition blockPos, Faces face, bool water);
	uint8_t vertexAO(bool s1, bool s2, bool corner);

private:
	float uMin, vMin, uMax, vMax;
	ChunkData blocks; // The chunk's block data

	void getUVFromAtlas(int index, int atlasSize, float& uMin, float& vMin, float& uMax, float& vMax);

	std::unordered_map<glm::ivec2, Chunk>* loadedChunks = nullptr;

	Mesh chunkMesh;
	Mesh waterMesh;

};