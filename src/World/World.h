#pragma once

#include <unordered_map>
#include <glm/glm.hpp>
#include "Chunk.h"
#include "../Camera.h" 
#include <glm/gtx/hash.hpp>
#include <future>
#include <queue>
#include <unordered_set>
#include "ThreadPool.h"
#include <mutex> 

class World 
{
public:
    std::unordered_map<glm::ivec2, std::future<Chunk>> futureChunkMap; 
    std::unordered_map<glm::ivec2, Chunk> loadedChunkMap;
    glm::ivec2 chunkPos;

    World();
    void reloadChunk(glm::ivec2 chunkPos);
    void updateChunks(glm::vec3 camPos);
    void renderChunks(Shader& shader, Shader& waterShader);
    uint8_t getBlockGlobal(glm::vec3 pos);
    void setBlockGlobal(glm::vec3 pos, uint8_t blockType);
    void Delete();

private:
    std::mutex chunkMapMutex;
    ThreadPool threadPool;
};