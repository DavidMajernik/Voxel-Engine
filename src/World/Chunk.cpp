#include "Chunk.h"

Texture* Chunk::texture = nullptr; // Initialize the static texture pointer to nullptr
FastNoiseLite Chunk::noiseGenerator; // Initialize the static noise generator

Chunk::Chunk() : chunkPos(glm::ivec3(0)), indexCount(0), chunkVAO(0), chunkVertexVBO(0), chunkUVVBO(0), chunkEBO(0) {
	// No need to call blocks() explicitly
}

Chunk::Chunk(glm::vec3 pos) : chunkPos(pos), indexCount(0), chunkVAO(0), chunkVertexVBO(0), chunkUVVBO(0), chunkEBO(0), blocks() {
	initializeTexture(); // Initialize the texture if not already done
	intitializeNoiseGenerator(); // Initialize the noise generator

	genBlocks(genHeightMap()); // Generate blocks for the chunk
	genFaces(); // Generate the faces for the chunk based on the blocks
}

int Chunk::getBlockIndex(const BlockPosition& blockPos) {
	// Calculate the 1D index from 3D block position
	return blockPos.x + blockPos.y * chunkSize + blockPos.z * chunkArea;
}

uint8_t Chunk::ChunkData::getBlock(const BlockPosition& blockPos) const {
	return blocks.at(Chunk::getBlockIndex(blockPos));
}

void Chunk::ChunkData::setBlock(const BlockPosition& blockPos, uint8_t blockType) {
	blocks.at(Chunk::getBlockIndex(blockPos)) = blockType;
}

std::vector<std::vector<float>> Chunk::genHeightMap()
{
	std::vector<std::vector<float>> heightMap = std::vector<std::vector<float>>(chunkSize, std::vector<float>(chunkSize));

	// Simple heightmap generation (for example, using Perlin noise or any other algorithm)
	for (int x = 0; x < chunkSize; x++) {
		for (int z = 0; z < chunkSize; z++) {
			float noiseValue = noiseGenerator.GetNoise(static_cast<float>(x + chunkPos.x), static_cast<float>(z + chunkPos.z));
			heightMap[x][z] = static_cast<int>((noiseValue + 1.0f) * 0.5f * chunkHeight);
		}
	}
	return heightMap;

}

void Chunk::genBlocks(std::vector<std::vector<float>> heightMap)
{

    for (int x = 0; x < chunkSize; x++) { // Loop through the x-axis of the chunk
        for (int z = 0; z < chunkSize; z++) {

            int columnHeight = static_cast<int>(heightMap[x][z]); // Get the height for the current column from the heightmap

            for (int y = 0; y < chunkHeight; y++) {

                if (y < columnHeight) {
					blocks.setBlock(BlockPosition(x, y, z), (y == columnHeight - 1) ? BlockType::GRASS : BlockType::DIRT);
                } else {
					blocks.setBlock(BlockPosition(x, y, z), BlockType::EMPTY);
                }
            }
        }
    }
   
}

void Chunk::genFaces() {
	auto start = std::chrono::high_resolution_clock::now();
	for (int x = 0; x < chunkSize; x++) {
		for (int z = 0; z < chunkSize; z++) {

			for (int y = 0; y < chunkHeight; y++) {

				int numFaces = 0;
				if (blocks.getBlock(BlockPosition(x, y, z)) != BlockType::EMPTY) {
					
					//Front faces
					//qualifications for front face: Block to the front is empty, is farthest front in chunk. 
					if (z < chunkSize - 1) {
						if (blocks.getBlock(BlockPosition(x, y, z + 1)) == BlockType::EMPTY) {
							integrateFace(BlockPosition(x, y, z), Faces::FRONT_F);
							numFaces++;
						}
					}
					else if (y < chunkHeight && blocks.getBlock(BlockPosition(x, y + 1, z)) == BlockType::EMPTY){
						// If it's the farthest front, we still need to render the front 
						integrateFace(BlockPosition(x, y, z), Faces::FRONT_F);
						numFaces++;
					}
					//Back faces
					//qualifications for back face: Block to the back is empty, is farthest back in chunk. 
					if (z > 0) {
						if (blocks.getBlock(BlockPosition(x, y, z-1)) == BlockType::EMPTY) {
							integrateFace(BlockPosition(x, y, z), Faces::BACK_F);
							numFaces++;
						}
					}
					else if (y < chunkHeight && blocks.getBlock(BlockPosition(x, y + 1, z)) == BlockType::EMPTY){
						// If it's the farthest back, we still need to render the back face
						integrateFace(BlockPosition(x, y, z), Faces::BACK_F);
						numFaces++;
					}
					//Left faces
					//qualifications for left face: Block to the left is empty, is farthest left in chunk. 
					if (x > 0) {
						if (blocks.getBlock(BlockPosition(x-1, y, z)) == BlockType::EMPTY) {
							integrateFace(BlockPosition(x, y, z), Faces::LEFT_F);
							numFaces++;
						}
					}
					else if (y < chunkHeight && blocks.getBlock(BlockPosition(x, y + 1, z)) == BlockType::EMPTY){
						// If it's the farthest left, we still need to render the left face
						integrateFace(BlockPosition(x, y, z), Faces::LEFT_F);
						numFaces++;
					}
					//Right faces
					//qualifications for right face: Block to the right is empty, is farthest right in chunk. 
					if (x < chunkSize - 1) {
						if (blocks.getBlock(BlockPosition(x + 1, y, z)) == BlockType::EMPTY) {
							integrateFace(BlockPosition(x, y, z), Faces::RIGHT_F);
							numFaces++;
						}
					}
					else if (y < chunkHeight && blocks.getBlock(BlockPosition(x, y + 1, z)) == BlockType::EMPTY){
						// If it's the farthest right, we still need to render the right face
						integrateFace(BlockPosition(x, y, z), Faces::RIGHT_F);
						numFaces++;
					}
					//Top faces
					//qualifications for top face: Block to the top is empty, is farthest top in chunk. 
					if (y < chunkHeight - 1) {
						if (blocks.getBlock(BlockPosition(x, y + 1, z)) == BlockType::EMPTY) {
							integrateFace(BlockPosition(x, y, z), Faces::TOP_F);
							numFaces++;
						}
					}
					else if (y < chunkHeight - 1 && blocks.getBlock(BlockPosition(x, y + 1, z)) == BlockType::EMPTY){
						// If it's the farthest top, we still need to render the top face
						integrateFace(BlockPosition(x, y, z), Faces::TOP_F);
						numFaces++;
					}
					//Bottom faces
					//qualifications for bottom face: Block to the bottom is empty, is farthest bottom in chunk. 
					if (y > 0) {
						if (blocks.getBlock(BlockPosition(x, y - 1, z)) == BlockType::EMPTY) {
							integrateFace(BlockPosition(x, y, z), Faces::BOTTOM_F);
							numFaces++;
						}
					}
					else if (y < chunkHeight - 1 && blocks.getBlock(BlockPosition(x, y + 1, z)) == BlockType::EMPTY){
						// If it's the farthest bottom, we still need to render the bottom face
						integrateFace(BlockPosition(x, y, z), Faces::BOTTOM_F);
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

void Chunk::integrateFace(BlockPosition blockPos, Faces face) {

	BlockType type = static_cast<BlockType>(blocks.getBlock(blockPos)); // Get the block type at the given position

	// Apply chunk position offset to vertices
	for (auto& vertex : rawVertexData.at(face)) {
		
        chunkVerts.push_back(glm::vec3(static_cast<float>(vertex.x + blockPos.x + chunkPos.x),
			static_cast<float>(vertex.y + blockPos.y + chunkPos.y),
			static_cast<float>(vertex.z + blockPos.z + chunkPos.z))); // Add the vertex to the chunkVerts vector

	}

	int texIndex = textureIndices.at(type)[face];
	getUVFromAtlas(texIndex, 16, uMin, vMin, uMax, vMax);

	chunkUVs.push_back(glm::vec2(uMin, vMin)); // Bottom Left
	chunkUVs.push_back(glm::vec2(uMax, vMin)); // Bottom Right
	chunkUVs.push_back(glm::vec2(uMax, vMax)); // Top Right
	chunkUVs.push_back(glm::vec2(uMin, vMax)); // Top Left

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
	GLenum error;
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

	glBindVertexArray(0);
}

void Chunk::render(Shader& shader)
{
	shader.use(); // Use the shader program
	glBindVertexArray(chunkVAO);
	texture->Bind(GL_TEXTURE0); // Bind the texture for the chunk

	glDrawElements(GL_TRIANGLES, chunkIndices.size(), GL_UNSIGNED_INT, 0); // Draw the chunk using the indices
	glBindVertexArray(0); // Unbind the VAO to avoid accidental modification
	GLenum error;
	if ((error = glGetError()) != GL_NO_ERROR) {
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

	if (texture != nullptr) {
		texture = nullptr;
	}
}

void Chunk::getUVFromAtlas(int index, int atlasSize, float& uMin, float& vMin, float& uMax, float& vMax)
{
	int col = index % atlasSize;
	int row = index / atlasSize;

	float spriteSize = 1.0f / atlasSize;

	uMin = col * spriteSize;
	vMin = row * spriteSize;
	uMax = uMin + spriteSize;
	vMax = vMin + spriteSize;
}
