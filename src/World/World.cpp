#include <iostream>
#include "World.h"

World::World() {
	// Constructor can be empty or initialize any necessary variables if needed
	chunkPos = glm::ivec2(0.0f, 0.0f); // Initialize chunk chunkPos to (0, 0)
	Chunk::initializeTexture();
	Chunk::intitializeNoiseGenerator(); // Initialize the texture and noise generator for chunks
	chunkBuildQueue = std::queue<Chunk>(); // Initialize the chunk build queue
	processingChunks = std::unordered_set<glm::ivec2>();
}

void World::updateChunks(glm::vec3 camPos)
{
	//std::cout << camera->Position.x << " " << camera->Position.y << " " << camera->Position.z << std::endl;
	chunkPos = glm::ivec2(
		static_cast<int>(camPos.x / chunkSize),
		static_cast<int>(camPos.z / chunkSize)
	);
	//std::cout << "Camera chunk chunkPos: " << chunkPos.x << ", " << chunkPos.y << std::endl;

	// Iterate over the render distance in both x and z directions
	for (int x = -renderDistance; x <= renderDistance; x++)
	{
		for (int z = -renderDistance; z <= renderDistance; z++)
		{

			glm::ivec2 chunkKey = glm::ivec2(chunkPos.x + x, chunkPos.y + z);
			// Check if the chunk already exists in the map
			if (loadedChunkMap.find(chunkKey) == loadedChunkMap.end() &&
				futureChunkMap.find(chunkKey) == futureChunkMap.end() &&
				processingChunks.find(chunkKey) == processingChunks.end())
			{
				// If it doesn't exist, create a new chunk at the calculated chunkPos
				futureChunkMap[chunkKey] = std::async(std::launch::async, [chunkKey]() {
					// Create a new chunk and build it
					std::cout << "Loading chunk at: " << chunkKey.x << ", " << chunkKey.y << std::endl;
					Chunk temp(glm::vec3(chunkKey.x * chunkSize, 0.0f, chunkKey.y * chunkSize));
					return temp;
					});

			}
		}
	}


	for (auto it = futureChunkMap.begin(); it != futureChunkMap.end(); )
	{
		if (it->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready) // Check if the future is ready
		{
			if (it->second.valid()) {
				processingChunks.insert(it->first);
				Chunk chunk = it->second.get(); // Store the result in a temporary variable
				//chunk.buildChunk();
				chunkBuildQueue.push(std::move(chunk)); // defer building // Use the temporary variable
				it = futureChunkMap.erase(it); // Remove from futureChunkMap
			}
		}
		else
		{
			++it; // Move to the next element
		}
	}

	// Build chunks from the queue
	int chunksBuilt = 0;
	int maxChunksPerFrame = 1;
	while (!chunkBuildQueue.empty() && chunksBuilt < maxChunksPerFrame)
	{
		Chunk chunk = std::move(chunkBuildQueue.front()); // Get the chunk from the queue
		chunk.buildChunk(); // Build the chunk
		std::cout << "Building chunk at: " << chunk.chunkPos.x << ", " << chunk.chunkPos.z << std::endl;
		glm::ivec2 key = glm::ivec2(chunk.chunkPos.x / chunkSize, chunk.chunkPos.z / chunkSize);
		loadedChunkMap[key] = std::move(chunk); // Store it in the loadedChunkMap
		chunkBuildQueue.pop(); // Remove it from the queue
		processingChunks.erase(key); // Remove from processingChunks set
		chunksBuilt++;
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