#include <iostream>
#include "World.h"

World::World() {
	// Constructor can be empty or initialize any necessary variables if needed
	chunkPos = glm::ivec2(0.0f, 0.0f); // Initialize chunk chunkPos to (0, 0)
	Chunk::initializeTexture();
	Chunk::cacheUVsFromAtlas();

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

			// Check if the chunk hasn't started loading yet
			if (loadedChunkMap.find(chunkKey) == loadedChunkMap.end() &&
				futureChunkMap.find(chunkKey) == futureChunkMap.end())
			{

				futureChunkMap[chunkKey] = std::async(std::launch::async, [this, chunkKey]() {
					Chunk chunk(glm::vec3(chunkKey.x * chunkSize, 0.0f, chunkKey.y * chunkSize)); 
					chunk.genFaces();
					return chunk;
					});


			}

		}

	}

	for (auto it = futureChunkMap.begin(); it != futureChunkMap.end(); ) {
		if (it->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			loadedChunkMap[it->first] = it->second.get();
			loadedChunkMap[it->first].uploadToGPU();
			it = futureChunkMap.erase(it);
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

