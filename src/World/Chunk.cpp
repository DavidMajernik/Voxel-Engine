#include <iostream>
#include "Chunk.h"


Texture* Chunk::texture = nullptr; // Initialize the static texture pointer to nullptr
FastNoiseLite Chunk::noiseGenerator; // Initialize the static noise generator

Chunk::Chunk() {
	position = glm::vec3(0.0f, 0.0f, 0.0f); // Default position
	chunkVerts = std::vector<glm::vec3>(); // Initialize the vertex vector for the chunk
	chunkUVs = std::vector<glm::vec2>(); // Initialize the UV vector for the chunk
	chunkIndices = std::vector<unsigned int>(); // Initialize the index vector for the chunk
	chunkBlocks = std::vector<Block>(SIZE * SIZE * HEIGHT);
}

Chunk::Chunk(glm::vec3 position)
{
	this->position = position;
	chunkVerts = std::vector<glm::vec3>(); // Initialize the vertex vector for the chunk
	chunkUVs = std::vector<glm::vec2>(); // Initialize the UV vector for the chunk
	chunkIndices = std::vector<unsigned int>(); // Initialize the index vector for the chunk
	chunkBlocks = std::vector<Block>(SIZE * SIZE * HEIGHT);
	genBlocks(genChunk()); // Generate blocks for the chunk
	genFaces(); // Generate the faces for the chunk based on the blocks
}

std::vector<std::vector<float>> Chunk::genChunk()
{
	std::vector<std::vector<float>> heightMap = std::vector<std::vector<float>>(SIZE, std::vector<float>(SIZE));

	// Simple heightmap generation (for example, using Perlin noise or any other algorithm)
	for (int x = 0; x < SIZE; x++) {
		for (int z = 0; z < SIZE; z++) {
			float noiseValue = noiseGenerator.GetNoise(static_cast<float>(x+position.x), static_cast<float>(z+position.z));
            heightMap[x][z] = static_cast<int>((noiseValue + 1.0f) * 0.5f * HEIGHT);
		}
	}
	return heightMap;

}

void Chunk::genBlocks(std::vector<std::vector<float>> heightMap)
{	
	auto start = std::chrono::high_resolution_clock::now();

	for (int x = 0; x < SIZE; x++) { // Loop through the x-axis of the chunk
		for (int z = 0; z < SIZE; z++) {
			
			int columnHeight = static_cast<int>(heightMap[x][z]); // Get the height for the current column from the heightmap

			for (int y = 0; y < HEIGHT; y++) {

				Block& block = getChunkBlocks(x, y, z); // Get the block at the current position
				if (y < columnHeight) {

					BlockData::BlockType blockType = (y == columnHeight - 1) ? BlockData::BlockType::GRASS : BlockData::BlockType::DIRT;

					block = Block(glm::vec3(x, y, z), blockType);
					
				}
				else { 
					block = Block(glm::vec3(x, y, z), BlockData::BlockType::EMPTY);
					
				}
			}
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "genBlocks time: " << duration.count() << " milliseconds" << std::endl;
}

void Chunk::genFaces() {
	auto start = std::chrono::high_resolution_clock::now();
	for (int x = 0; x < SIZE; x++) {
		for (int z = 0; z < SIZE; z++) {

			for (int y = 0; y < HEIGHT; y++) {

				int numFaces = 0; 
				if (getChunkBlocks(x, y, z).type != BlockData::BlockType::EMPTY) {
					//Front faces
					//qualifications for front face: Block to the front is empty, is farthest front in chunk. 
					if (z < SIZE - 1) {
						if (getChunkBlocks(x, y, z+1).type == BlockData::BlockType::EMPTY) {
							integrateFace(getChunkBlocks(x, y, z), BlockData::Faces::FRONT);
							numFaces++;
						}
					}
					else if (y < HEIGHT && getChunkBlocks(x, y + 1, z).type == BlockData::BlockType::EMPTY) {
						// If it's the farthest front, we still need to render the front face
						integrateFace(getChunkBlocks(x, y, z), BlockData::Faces::FRONT);
						numFaces++;
					}
					//Back faces
					//qualifications for back face: Block to the back is empty, is farthest back in chunk. 
					if (z > 0) {
						if (getChunkBlocks(x, y, z - 1).type == BlockData::BlockType::EMPTY) {
							integrateFace(getChunkBlocks(x, y, z), BlockData::Faces::BACK);
							numFaces++;
						}
					}
					else if (y < HEIGHT && getChunkBlocks(x, y + 1, z).type == BlockData::BlockType::EMPTY) {
						// If it's the farthest back, we still need to render the back face
						integrateFace(getChunkBlocks(x, y, z), BlockData::Faces::BACK);
						numFaces++;
					}
					//Left faces
					//qualifications for left face: Block to the left is empty, is farthest left in chunk. 
					if (x > 0) {
						if (getChunkBlocks(x - 1, y, z).type == BlockData::BlockType::EMPTY) {
							integrateFace(getChunkBlocks(x, y, z), BlockData::Faces::LEFT);
							numFaces++;
						}
					}
					else if(y < HEIGHT-1 && getChunkBlocks(x, y + 1, z).type == BlockData::BlockType::EMPTY){
						// If it's the farthest left, we still need to render the left face
						integrateFace(getChunkBlocks(x, y, z), BlockData::Faces::LEFT);
						numFaces++;
					}
					//Right faces
					//qualifications for right face: Block to the right is empty, is farthest right in chunk. 
					if (x < SIZE-1) {
						if (getChunkBlocks(x + 1, y, z).type == BlockData::BlockType::EMPTY) {
							integrateFace(getChunkBlocks(x, y, z), BlockData::Faces::RIGHT);
							numFaces++;
						}
					}
					else if (y < HEIGHT && getChunkBlocks(x, y + 1, z).type == BlockData::BlockType::EMPTY) {
						// If it's the farthest right, we still need to render the right face
						integrateFace(getChunkBlocks(x, y, z), BlockData::Faces::RIGHT);
						numFaces++;
					}
					//Top faces
					//qualifications for top face: Block to the top is empty, is farthest top in chunk. 
					if (y < HEIGHT - 1) {
						if (getChunkBlocks(x, y + 1, z).type == BlockData::BlockType::EMPTY) {
							integrateFace(getChunkBlocks(x, y, z), BlockData::Faces::TOP);
							numFaces++;
						}
					}
					else {
						// If it's the farthest top, we still need to render the top face
						integrateFace(getChunkBlocks(x, y, z), BlockData::Faces::TOP);
						numFaces++;
					}
					//Bottom faces
					//qualifications for bottom face: Block to the bottom is empty, is farthest bottom in chunk. 
					if (y > 0) {
						if (getChunkBlocks(x, y-1, z).type == BlockData::BlockType::EMPTY) {
							integrateFace(getChunkBlocks(x, y, z), BlockData::Faces::BOTTOM);
							numFaces++;
						}
					}
					else {
						// If it's the farthest bottom, we still need to render the bottom face
						integrateFace(getChunkBlocks(x, y, z), BlockData::Faces::BOTTOM);
						numFaces++;
					}

				}

				addIndices(numFaces);

			}

		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "GenFaces time: " << duration.count() << " milliseconds" << std::endl;

}


void Chunk::integrateFace(Block block, BlockData::Faces face) {
	auto faceData = block.GetFace(face);

	// Apply chunk position offset to vertices
	for (auto& vertex : faceData.vertices) {
		vertex += position; // Offset the vertex by the chunk's 
		chunkVerts.push_back(vertex); // Add the vertex to the chunkVerts vector
		//std::cout << vertex.x << " " << vertex.z  << std::endl;
	}
	//chunkVerts.insert(chunkVerts.end(), faceData.vertices.begin(), faceData.vertices.end());
	chunkUVs.insert(chunkUVs.end(), faceData.uvs.begin(), faceData.uvs.end());
}

void Chunk::addIndices(int amtFaces)
{
	for (int i = 0; i < amtFaces; i++) {
		chunkIndices.push_back(0 + indexCount);
		chunkIndices.push_back(1 + indexCount);
		chunkIndices.push_back(2 + indexCount);
		chunkIndices.push_back(2 + indexCount);
		chunkIndices.push_back(3 + indexCount);
		chunkIndices.push_back(0 + indexCount);

		indexCount += 4; // Each face has 4 vertices, so we increment by 4
	}
}

void Chunk::buildChunk()
{
	glGenVertexArrays(1, &chunkVAO); // Generate a Vertex Array Object for the chunk
	glBindVertexArray(chunkVAO); // Bind the VAO

	glGenBuffers(1, &chunkVertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, chunkVertexVBO); // Bind the vertex buffer
	glBufferData(GL_ARRAY_BUFFER, chunkVerts.size() * sizeof(glm::vec3), &chunkVerts[0], GL_STATIC_DRAW); // Load the vertex data into the buffer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Set the vertex attribute pointer
	glEnableVertexAttribArray(0); // Enable the vertex attribute array
	
	glGenBuffers(1, &chunkUVVBO);
	glBindBuffer(GL_ARRAY_BUFFER, chunkUVVBO); // Bind the vertex buffer
	glBufferData(GL_ARRAY_BUFFER, chunkUVs.size() * sizeof(glm::vec2), &chunkUVs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &chunkEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunkEBO); // Bind the index buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunkIndices.size() * sizeof(unsigned int), &chunkIndices[0], GL_STATIC_DRAW);

}

void Chunk::render(Shader& shader)
{
	shader.use(); // Use the shader program
	glBindVertexArray(chunkVAO);
	texture->Bind(GL_TEXTURE0); // Bind the texture for the chunk

	glDrawElements(GL_TRIANGLES, chunkIndices.size(), GL_UNSIGNED_INT, 0); // Draw the chunk using the indices
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL Error: " << error << std::endl;
	}
}

void Chunk::initializeTexture() {
	if (texture == nullptr) {
		texture = new Texture(GL_TEXTURE_2D, "assets/MinecraftAtlas.png");
		texture->Load();
	}
}

void Chunk::intitializeNoiseGenerator() {
	noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin); // Set the noise type to Perlin noise
	noiseGenerator.SetFrequency(0.02f);
}

void Chunk::cleanupTexture() {
	if (texture != nullptr) {
		delete texture;
		texture = nullptr;
	}
}

void Chunk::Delete()
{
	glDeleteVertexArrays(1, &chunkVAO);
	glDeleteBuffers(1, &chunkVertexVBO);
	glDeleteBuffers(1, &chunkUVVBO);
	glDeleteBuffers(1, &chunkEBO);

	delete texture;
}



