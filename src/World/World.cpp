#include <iostream>
#include "World.h"

World::World() : threadPool() {
	chunkPos = glm::ivec2(0.0f, 0.0f); // Initialize chunk chunkPos to (0, 0)
	Chunk::initializeTexture();
	Chunk::cacheUVsFromAtlas();
}

void World::reloadChunk(glm::ivec2 chunkPos) 
{
	loadedChunkMap[chunkPos].Delete();
	loadedChunkMap[chunkPos].genFaces();
	loadedChunkMap[chunkPos].uploadToGPU();
}

uint8_t World::getBlockGlobal(glm::vec3 pos)
{
	int paddingOffset = 1;
	int chunkX = static_cast<int>(std::floor((pos.x - paddingOffset) / chunkSize));
	int chunkZ = static_cast<int>(std::floor((pos.z - paddingOffset) / chunkSize));
	glm::ivec2 chunkPos(chunkX, chunkZ);

	int localX = static_cast<int>(pos.x) - chunkX * chunkSize + paddingOffset;
	int localY = static_cast<int>(pos.y);
	int localZ = static_cast<int>(pos.z) - chunkZ * chunkSize + paddingOffset;
	glm::vec3 localBlockPos(localX, localY, localZ);

	return loadedChunkMap[chunkPos].getBlock(localBlockPos);
}

void World::setBlockGlobal(glm::vec3 pos, uint8_t blockType)
{
	int paddingOffset = 1;
	int baseChunkX = static_cast<int>(std::floor((pos.x - paddingOffset) / chunkSize));
	int baseChunkZ = static_cast<int>(std::floor((pos.z - paddingOffset) / chunkSize));

	// Track which chunks need to be reloaded
	std::unordered_set<glm::ivec2> chunksToReload;

	// Check all 4 possible overlapping chunks
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

				if (loadedChunkMap.count(chunkPos)) {
					loadedChunkMap[chunkPos].setBlock(localBlockPos, blockType);
					chunksToReload.insert(chunkPos);

					// If the block is on the main area border, reload the neighbor too
					if (localX == 1 || localX == chunkSize ||
						localZ == 1 || localZ == chunkSize) {
						// Check all 4 directions
						if (localX == 1) chunksToReload.insert(glm::ivec2(chunkX - 1, chunkZ));
						if (localX == chunkSize) chunksToReload.insert(glm::ivec2(chunkX + 1, chunkZ));
						if (localZ == 1) chunksToReload.insert(glm::ivec2(chunkX, chunkZ - 1));
						if (localZ == chunkSize) chunksToReload.insert(glm::ivec2(chunkX, chunkZ + 1));
					}
				}
			}
		}
	}

	// Reload all affected chunks
	for (const auto& chunkPos : chunksToReload) {
		if (loadedChunkMap.count(chunkPos)) {
			reloadChunk(chunkPos);
		}
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

			//// Check if the chunk hasn't started loading yet
			//if (loadedChunkMap.find(chunkKey) == loadedChunkMap.end() &&
			//	futureChunkMap.find(chunkKey) == futureChunkMap.end())
			//{


			//	// Use the thread pool to load the chunk asynchronously
			//	futureChunkMap[chunkKey] = threadPool.enqueue([this, chunkKey]() {
			//		Chunk chunk(glm::vec3(chunkKey.x * chunkSize, 0.0f, chunkKey.y * chunkSize));
			//		chunk.genFaces();
			//		return chunk;
			//		});

			//}

			if (loadedChunkMap.find(chunkKey) == loadedChunkMap.end()) {
				loadedChunkMap[chunkKey] = Chunk(glm::vec3(chunkKey.x * chunkSize, 0.0f, chunkKey.y * chunkSize));
				loadedChunkMap[chunkKey].genFaces();
				loadedChunkMap[chunkKey].uploadToGPU();
			}

		}

	}

	/*for (auto it = futureChunkMap.begin(); it != futureChunkMap.end(); ) {
		if (it->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			loadedChunkMap[it->first] = it->second.get();
			loadedChunkMap[it->first].uploadToGPU();
			it = futureChunkMap.erase(it);
		}
		else {
			++it;
		}
	}*/

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

	// Unload chunks that are outside the render distance
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

void World::renderChunks(Shader& shader)
{

	// Render all chunks in the world
	for (auto& pair : loadedChunkMap)
	{
		pair.second.render(shader); // Call render on each chunk
	}


}

void World::Delete() {
	// Clean up all chunks in the world
	for (auto& pair : loadedChunkMap)
	{
		pair.second.Delete(); // Call Delete on each chunk to free resources
	}
	loadedChunkMap.clear(); // Clear the map to remove all references
	Chunk::cleanupTexture(); // Clean up the texture if needed
	futureChunkMap.clear();
}

