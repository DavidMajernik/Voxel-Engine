#include <iostream>
#include "World.h"

World::World() {
	// Constructor can be empty or initialize any necessary variables if needed
	chunkPos = glm::ivec2(0.0f, 0.0f); // Initialize chunk chunkPos to (0, 0)
	processingChunks = std::unordered_set<glm::ivec2>();
	Chunk::initializeTexture();
	Terrain::initializeNoiseGenerator();
	Chunk::cacheUVsFromAtlas();

}

void World::updateChunks(glm::vec3 camPos)
{

	//std::cout << camera->Position.x << " " << camera->Position.y << " " << camera->Position.z << std::endl;
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
			// Check if the chunk already exists in the map
			if (loadedChunkMap.find(chunkKey) == loadedChunkMap.end() &&
				processingChunks.find(chunkKey) == processingChunks.end())
			{
				//// If it doesn't exist, create a new chunk at the calculated chunkPos
				processingChunks.insert(chunkKey);
				loadedChunkMap[chunkKey] = Chunk(glm::vec3(chunkKey.x * chunkSize, 0.0f, chunkKey.y * chunkSize), &loadedChunkMap);
				try {
					for (const auto& offset : neighborOffsets) {
						glm::vec2 neighborKey = glm::vec2(chunkKey.x + offset.x, chunkKey.y + offset.y);
						auto neighborIt = loadedChunkMap.find(neighborKey);
						if (neighborIt != loadedChunkMap.end()) {

							neighborIt->second.isGenerated = false; // Force rebuild neighbors of a newly loaded 
						}
					}
				}
				catch (const std::exception& e) {
					// Handle the exception or log it
					std::cerr << "Exception: " << e.what() << std::endl;
				}
				processingChunks.erase(chunkKey); // Remove from processing set

			}
		}
	}

	
	//auto start = std::chrono::high_resolution_clock::now();
    for (auto it = loadedChunkMap.begin(); it != loadedChunkMap.end(); ) {
        Chunk& chunkRef = it->second;
        glm::ivec2 chunkCoord(chunkRef.chunkPos.x / chunkSize, chunkRef.chunkPos.z / chunkSize);

        if (!chunkRef.isGenerated) {
			chunkRef.Delete();
			chunkRef.genFaces();
			chunkRef.buildChunk();
			chunkRef.isGenerated = true;
        }
        it++;
    }
	/*auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "GenFaces time: " << duration.count() << " milliseconds" << std::endl;*/


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
			// Outside of active range -> unload it
			it->second.Delete();  // Make sure this cleans up OpenGL buffers
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
	processingChunks.clear();
}