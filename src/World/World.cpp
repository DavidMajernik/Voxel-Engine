#include <iostream>
#include "World.h"

World::World() : threadPool() {
	chunkPos = glm::ivec2(0.0f, 0.0f); // Initialize chunk chunkPos to (0, 0)
	Chunk::initializeTexture();
	Chunk::cacheUVsFromAtlas();
}

void World::reloadChunk(glm::ivec2 chunkPos) {
	std::lock_guard<std::mutex> lock(chunkMapMutex);
	if (loadedChunkMap.find(chunkPos) != loadedChunkMap.end()) {
		loadedChunkMap[chunkPos].Delete();
		loadedChunkMap[chunkPos].genFaces();
		loadedChunkMap[chunkPos].uploadToGPU();
	}
}

uint8_t World::getBlockGlobal(glm::vec3 pos) {
    int paddingOffset = 1;
    int chunkX = static_cast<int>(std::floor((pos.x - paddingOffset) / chunkSize));
    int chunkZ = static_cast<int>(std::floor((pos.z - paddingOffset) / chunkSize));
    glm::ivec2 chunkPos(chunkX, chunkZ);

    int localX = static_cast<int>(pos.x) - chunkX * chunkSize + paddingOffset;
    int localY = static_cast<int>(pos.y);
    int localZ = static_cast<int>(pos.z) - chunkZ * chunkSize + paddingOffset;
    glm::vec3 localBlockPos(localX, localY, localZ);

    std::lock_guard<std::mutex> lock(chunkMapMutex);

    if (loadedChunkMap.find(chunkPos) != loadedChunkMap.end()) {
        return loadedChunkMap[chunkPos].getBlock(localBlockPos);
    } else {
        return BlockType::EMPTY; // Default block if not loaded
    }
}

void World::setBlockGlobal(glm::vec3 pos, uint8_t blockType) {
	int paddingOffset = 1;
	int baseChunkX = static_cast<int>(std::floor((pos.x - paddingOffset) / chunkSize));
	int baseChunkZ = static_cast<int>(std::floor((pos.z - paddingOffset) / chunkSize));

	std::unordered_set<glm::ivec2> chunksToReload;

	for (int dx = 0; dx <= 1; ++dx) {
		for (int dz = 0; dz <= 1; ++dz) {
			int chunkX = baseChunkX + dx;
			int chunkZ = baseChunkZ + dz;
			glm::ivec2 chunkPos(chunkX, chunkZ);

			int chunkOriginX = chunkX * chunkSize;
			int chunkOriginZ = chunkZ * chunkSize;

			int minX = chunkOriginX - paddingOffset;
			int maxX = chunkOriginX + chunkSize + paddingOffset - 1;
			int minZ = chunkOriginZ - paddingOffset;
			int maxZ = chunkOriginZ + chunkSize + paddingOffset - 1;

			if (pos.x >= minX && pos.x <= maxX && pos.z >= minZ && pos.z <= maxZ) {
				int localX = static_cast<int>(pos.x) - chunkOriginX + paddingOffset;
				int localY = static_cast<int>(pos.y);
				int localZ = static_cast<int>(pos.z) - chunkOriginZ + paddingOffset;
				glm::vec3 localBlockPos(localX, localY, localZ);

				std::lock_guard<std::mutex> lock(chunkMapMutex);
				if (loadedChunkMap.find(chunkPos) != loadedChunkMap.end()) {
					loadedChunkMap[chunkPos].setBlock(localBlockPos, blockType);
					chunksToReload.insert(chunkPos);

					if (localX == 1 || localX == chunkSize ||
						localZ == 1 || localZ == chunkSize) {
						if (localX == 1) chunksToReload.insert(glm::ivec2(chunkX - 1, chunkZ));
						if (localX == chunkSize) chunksToReload.insert(glm::ivec2(chunkX + 1, chunkZ));
						if (localZ == 1) chunksToReload.insert(glm::ivec2(chunkX, chunkZ - 1));
						if (localZ == chunkSize) chunksToReload.insert(glm::ivec2(chunkX, chunkZ + 1));
					}
				}
			}
		}
	}

	for (const auto& chunkPos : chunksToReload) {
		reloadChunk(chunkPos);
	}
}

void World::updateChunks(glm::vec3 camPos)
{

	chunkPos = glm::ivec2(
		static_cast<int>(camPos.x / chunkSize),
		static_cast<int>(camPos.z / chunkSize)
	);

	// Iterate over the render distance and load chunks
	for (int x = -renderDistance; x <= renderDistance; x++)
	{
		for (int z = -renderDistance; z <= renderDistance; z++)
		{


			glm::ivec2 chunkKey = glm::ivec2(chunkPos.x + x, chunkPos.y + z);

			std::lock_guard<std::mutex> lock(chunkMapMutex);
			// Check if the chunk hasn't started loading yet
			if (loadedChunkMap.find(chunkKey) == loadedChunkMap.end() &&
				futureChunkMap.find(chunkKey) == futureChunkMap.end())
			{

				// Use the thread pool to load the chunk asynchronously
				futureChunkMap[chunkKey] = threadPool.enqueue([this, chunkKey]() {
					Chunk chunk(glm::vec3(chunkKey.x * chunkSize, 0.0f, chunkKey.y * chunkSize));
					chunk.genFaces();
					return chunk;
					});

			}

		}

	}

	for (auto it = futureChunkMap.begin(); it != futureChunkMap.end(); ) {
		if (it->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			auto chunkKey = it->first;
			auto future = std::move(it->second);
			it = futureChunkMap.erase(it); // erase first

			try {
				Chunk chunk = future.get(); // call outside lock
				std::lock_guard<std::mutex> lock(chunkMapMutex);
				loadedChunkMap[chunkKey] = std::move(chunk);
				loadedChunkMap[chunkKey].uploadToGPU();
			}
			catch (...) {
				std::cout << "Error in Chunkmap future" << std::endl;
			}
		}
		else {
			++it;
		}
	}

	// Iterate over the buffer distance and add to buffered set
	std::unordered_set<glm::ivec2> bufferedChunkSet;

	for (int x = -unloadDistance; x <= unloadDistance; x++)
	{
		for (int z = -unloadDistance; z <= unloadDistance; z++)
		{
			glm::ivec2 chunkKey = glm::ivec2(chunkPos.x + x, chunkPos.y + z);
			bufferedChunkSet.insert(chunkKey);
		}
	}

	{
		std::lock_guard<std::mutex> lock(chunkMapMutex);
		for (auto it = loadedChunkMap.begin(); it != loadedChunkMap.end(); ) {
			if (bufferedChunkSet.find(it->first) == bufferedChunkSet.end()) {
				it->second.Delete();
				it = loadedChunkMap.erase(it);
			}
			else {
				++it;
			}
		}
	}


}

void World::renderChunks(Shader& shader) {
	std::lock_guard<std::mutex> lock(chunkMapMutex);
	//render solid blocks first
	shader.setBool("renderingWater", false);
	shader.setBool("renderingBillboard", false);
	for (auto& pair : loadedChunkMap) {
		pair.second.renderSolids(shader);
	}

	//billboards
	shader.setBool("renderingBillboard", true);
	for (auto& pair : loadedChunkMap) {
		pair.second.renderBillboards(shader);
	}
	
	//render transparent blocks 
	shader.setBool("renderingBillboard", false);
	shader.setBool("renderingWater", true);
	for (auto& pair : loadedChunkMap) {
		pair.second.renderWater(shader);
	}
}

void World::Delete() {
	std::lock_guard<std::mutex> lock(chunkMapMutex);
	for (auto& pair : loadedChunkMap)
		pair.second.Delete();
	loadedChunkMap.clear();
	futureChunkMap.clear();
	Chunk::cleanupTexture();
}

