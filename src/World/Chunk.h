#pragma once
#include "Block.h"
#include "../Shader.h"
#include "../Texture.h"
#include "../dependencies/FastNoiseLite.h"
#include <chrono>

class Chunk
{
public:
	static const int SIZE = 32; 
	static const int HEIGHT = 32;
	glm::vec3 position;

	unsigned int chunkVAO; // Vertex Array Object for the chunk
	unsigned int chunkVertexVBO;
	unsigned int chunkUVVBO;
	unsigned int chunkEBO;
	static Texture* texture;
	static FastNoiseLite noiseGenerator; // Noise generator for heightmap

	Chunk();
	Chunk(glm::vec3 position);

	std::vector<std::vector<float>> genChunk(); // generate the data

	void genBlocks(std::vector<std::vector<float>> heightMap); // generate the appropriate block faces given the data

	void genFaces();

	void buildChunk(); // take data and process it for rendering

	void render(Shader& shader); // Drawing the chunk

	void addIndices(int amtFaces); // Add the amount of indices between a range

	void integrateFace(Block block, BlockData::Faces face); // Integrate a face into the chunk data

	void Delete();

	inline Block& getChunkBlocks(int x, int y, int z) {
		return chunkBlocks[x + SIZE * (y + HEIGHT * z)];
	}

	static void initializeTexture();
	static void cleanupTexture();

	static void intitializeNoiseGenerator();

	

private:
	std::vector<glm::vec3> chunkVerts;
	std::vector<glm::vec2> chunkUVs;
	std::vector<unsigned int> chunkIndices;

	unsigned int indexCount;
	std::vector<Block> chunkBlocks;


};