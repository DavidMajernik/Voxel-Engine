#pragma once

#include <unordered_map>
#include <glm/glm.hpp>
#include "Chunk.h"
#include "../Camera.h" 
#include <glm/gtx/hash.hpp>
#include <future>
#include <queue>
#include <unordered_set>


class World  // A class to manage the world and its chunks
{

public:

	std::unordered_map<glm::ivec2, std::future<Chunk>> futureChunkMap; 
	std::unordered_map<glm::ivec2, Chunk> loadedChunkMap;
	std::unordered_set<glm::ivec2> processingChunks;
	glm::ivec2 chunkPos;

	World();
	void updateChunks(glm::vec3 camPos);
	void renderChunks(Shader& shader);
	void Delete();

private:

};