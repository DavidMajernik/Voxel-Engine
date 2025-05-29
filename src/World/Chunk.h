#pragma once

#include "World_Constants.h"
#include <unordered_map>
#include <glm/gtx/hash.hpp>
#include <array>
#include <glm/vec3.hpp>
#include <glm/ext/vector_float2.hpp>
#include "../Texture.h"
#include "../Shader.h"
#include <iostream>
#include <chrono>
#include "Terrain.h"

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
	void genFaces();
	void integrateFace(BlockPosition blockPos, Faces face);
	void addIndices(int amtFaces, bool water);
	void uploadToGPU();
	void render(Shader& shader, Shader& waterShader);
	void Delete();

	void setBlock(const BlockPosition& blockPos, uint8_t blockType);
	uint8_t getBlock(const BlockPosition& blockPos) const;

	void generateAOVals(BlockPosition blockPos, Faces face, bool water);
	uint8_t vertexAO(bool s1, bool s2, bool corner);

private:
	float uMin, vMin, uMax, vMax;
	ChunkData blocks; // The chunk's block data

	void getUVFromAtlas(int index, int atlasSize, float& uMin, float& vMin, float& uMax, float& vMax);

	unsigned int chunkVAO; // Vertex Array Object for the chunk
	unsigned int chunkVertexVBO;
	unsigned int chunkUVVBO;
	unsigned int chunkEBO;
	unsigned int chunkAOBO;

	std::unique_ptr < std::vector<glm::vec3>> chunkVerts;
	std::unique_ptr < std::vector<glm::vec2>> chunkUVs;
	std::unique_ptr < std::vector<unsigned int>> chunkIndices;
	std::unique_ptr<std::vector<uint8_t>> AOVals;

	unsigned int waterVAO; // Vertex Array Object for the chunk
	unsigned int waterVertexVBO;
	unsigned int waterUVVBO;
	unsigned int waterEBO;
	unsigned int waterAOBO;

	std::unique_ptr < std::vector<glm::vec3>> waterVerts;
	std::unique_ptr < std::vector<glm::vec2>> waterUVs;
	std::unique_ptr < std::vector<unsigned int>> waterIndices;
	std::unique_ptr<std::vector<uint8_t>> waterAOVals;

	std::unordered_map<glm::ivec2, Chunk>* loadedChunks = nullptr;

	int indexCount;
	int waterIndexCount;
};