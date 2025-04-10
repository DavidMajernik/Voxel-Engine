#include "Chunk.h"

Texture* Chunk::texture = nullptr; // Initialize the static texture pointer to nullptr
FastNoiseLite Chunk::noiseGenerator; // Initialize the static noise generator
std::array<std::array<std::array<float, 4>, 6>, 256> Chunk::cachedUVs;

Chunk::Chunk() : chunkPos(glm::ivec3(0)), indexCount(0), chunkVAO(0), chunkVertexVBO(0), chunkUVVBO(0), chunkEBO(0), chunkAOBO(0) {
	// No need to call blocks() explicitly
	AOVals = std::make_unique<std::vector<uint8_t>>();

}
Chunk::Chunk(glm::vec3 pos, std::unordered_map<glm::ivec2, Chunk>* loadedChunkMap) : chunkPos(pos), indexCount(0), chunkVAO(0), chunkVertexVBO(0), chunkUVVBO(0), chunkEBO(0), blocks(), chunkAOBO(0), loadedChunks(loadedChunkMap) {
	chunkVerts = std::make_unique<std::vector<glm::vec3>>();
	chunkUVs = std::make_unique<std::vector<glm::vec2>>();
	chunkIndices = std::make_unique<std::vector<unsigned int>>();
	AOVals = std::make_unique<std::vector<uint8_t>>();
	genBlocks(genHeightMap()); // Generate blocks for the chunk
	//genFaces(); // Generate the faces for the chunk based on the blocks

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
				}
				else {
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
				BlockPosition current = BlockPosition(x, y, z);
				if (blocks.getBlock(current) != BlockType::EMPTY) {

					//Front faces
					//qualifications for front face: Block to the front is empty
					if (getBlockGlobal(BlockPosition(x, y, z + 1)) == BlockType::EMPTY && getBlockGlobal(BlockPosition(x, y+1, z)) == BlockType::EMPTY) {
						integrateFace(current, Faces::FRONT_F);
						numFaces++;
					}

					//Back faces
					//qualifications for back face: Block to the back is empty
					if (getBlockGlobal(BlockPosition(x, y, z - 1)) == BlockType::EMPTY && getBlockGlobal(BlockPosition(x, y + 1, z)) == BlockType::EMPTY) {
						integrateFace(current, Faces::BACK_F);
						numFaces++;

					}
					
					//Left faces
					//qualifications for left face: Block to the left is empty
					if (getBlockGlobal(BlockPosition(x - 1, y, z)) == BlockType::EMPTY && getBlockGlobal(BlockPosition(x, y + 1, z)) == BlockType::EMPTY) {
						integrateFace(current, Faces::LEFT_F);
						numFaces++;

					}
					//Right faces
					//qualifications for right face: Block to the right is empty
					if (getBlockGlobal(BlockPosition(x + 1, y, z)) == BlockType::EMPTY && getBlockGlobal(BlockPosition(x, y + 1, z)) == BlockType::EMPTY) {
						integrateFace(current, Faces::RIGHT_F);
						numFaces++;

					}
					//Top faces
					//qualifications for top face: Block to the top is empty, is farthest top in chunk. 
					if (y < chunkHeight - 1) {
						if (blocks.getBlock(BlockPosition(x, y + 1, z)) == BlockType::EMPTY) {
							integrateFace(current, Faces::TOP_F);
							numFaces++;

						}
					}
					else {
						// If it's the farthest top, we still need to render the top face
						integrateFace(current, Faces::TOP_F);
						numFaces++;

					}
					//Bottom faces
					//qualifications for bottom face: Block to the bottom is empty
					if (y > 0) {
						if (blocks.getBlock(BlockPosition(x, y - 1, z)) == BlockType::EMPTY) {
							integrateFace(current, Faces::BOTTOM_F);
							numFaces++;

						}
					}
					else if (blocks.getBlock(BlockPosition(x, y + 1, z)) == BlockType::EMPTY) {
						// If it's the farthest bottom, we still need to render the bottom face
						integrateFace(current, Faces::BOTTOM_F);
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

		chunkVerts->push_back(glm::vec3(static_cast<float>(vertex.x + blockPos.x + chunkPos.x),
			static_cast<float>(vertex.y + blockPos.y + chunkPos.y),
			static_cast<float>(vertex.z + blockPos.z + chunkPos.z))); // Add the vertex to the chunkVerts vector

	}

	const auto& uv = cachedUVs[static_cast<int>(type)][face];

	chunkUVs->emplace_back(uv[0], uv[1]); // uMin, vMin
	chunkUVs->emplace_back(uv[2], uv[1]); // uMax, vMin
	chunkUVs->emplace_back(uv[2], uv[3]); // uMax, vMax
	chunkUVs->emplace_back(uv[0], uv[3]); // uMin, vMax

	//add ao vals
	generateAOVals(blockPos, face);

}


void Chunk::addIndices(int amtFaces)
{
	for (int i = 0; i < amtFaces; i++) {
		chunkIndices->push_back(0 + indexCount);
		chunkIndices->push_back(1 + indexCount);
		chunkIndices->push_back(2 + indexCount);
		chunkIndices->push_back(2 + indexCount);
		chunkIndices->push_back(3 + indexCount);
		chunkIndices->push_back(0 + indexCount);

		indexCount += 4; // Each face has 4 vertices, so we increment by 4
	}
}

void Chunk::buildChunk()
{
	//std::cout << "Verts: " << chunkVerts->size() << " | AO: " << AOVals->size() << std::endl;

	glGenVertexArrays(1, &chunkVAO); // Generate a Vertex Array Object for the chunk
	glBindVertexArray(chunkVAO); // Bind the VAO


	glGenBuffers(1, &chunkVertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, chunkVertexVBO); // Bind the vertex buffer
	glBufferData(GL_ARRAY_BUFFER, chunkVerts->size() * sizeof(glm::vec3), chunkVerts->data(), GL_STATIC_DRAW); // Load the vertex data into the buffer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Set the vertex attribute pointer
	glEnableVertexAttribArray(0); // Enable the vertex attribute array

	glGenBuffers(1, &chunkUVVBO);
	glBindBuffer(GL_ARRAY_BUFFER, chunkUVVBO); // Bind the vertex buffer
	glBufferData(GL_ARRAY_BUFFER, chunkUVs->size() * sizeof(glm::vec2), chunkUVs->data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &chunkEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunkEBO); // Bind the index buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunkIndices->size() * sizeof(unsigned int), chunkIndices->data(), GL_STATIC_DRAW);

	glGenBuffers(1, &chunkAOBO);
	glBindBuffer(GL_ARRAY_BUFFER, chunkAOBO);
	glBufferData(GL_ARRAY_BUFFER, AOVals->size() * sizeof(uint8_t), AOVals->data(), GL_STATIC_DRAW);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(uint8_t), (void*)0); // Correctly interpret as integer
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}


void Chunk::render(Shader& shader)
{
	shader.use(); // Use the shader program
	glBindVertexArray(chunkVAO);
	texture->Bind(GL_TEXTURE0); // Bind the texture for the chunk

	glDrawElements(GL_TRIANGLES, chunkIndices->size(), GL_UNSIGNED_INT, 0); // Draw the chunk using the indices
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
	glDeleteBuffers(1, &chunkAOBO);

	indexCount = 0;
	chunkVerts->clear();
	chunkUVs->clear();
	chunkIndices->clear();
	AOVals->clear();
}

void Chunk::cacheUVsFromAtlas() {
	int atlasSize = 16; // Adjust if your atlas isn't 16x16
	float spriteSize = 1.0f / static_cast<float>(atlasSize);

	for (const auto& textureIndex : textureIndices) {
		auto BlockType = textureIndex.first;
		auto faceIndices = textureIndex.second;
		for (int face = 0; face < 6; ++face) {
			int texIndex = faceIndices[face];

			int col = texIndex % atlasSize;
			int row = texIndex / atlasSize;

			float uMin = col * spriteSize;
			float vMin = row * spriteSize;
			float uMax = uMin + spriteSize;
			float vMax = vMin + spriteSize;

			cachedUVs[static_cast<int>(BlockType)][face] = { uMin, vMin, uMax, vMax };
		}
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

void Chunk::generateAOVals(BlockPosition blockPos, Faces face) {

	// Corner offsets
	glm::vec3 corners[6][4] = {
		// FRONT_F
		{{1, 1, 1}, {-1, 1, 1}, {-1, -1, 1}, {1, -1, 1}},
		// BACK_F
		{{-1, 1, -1}, {1, 1, -1}, {1, -1, -1}, {-1, -1, -1}},
		// LEFT_F
		{{-1, 1, -1}, {-1, 1, 1}, {-1, -1, 1}, {-1, -1, -1}},
		// RIGHT_F
		{{1, 1, 1}, {1, 1, -1}, {1, -1, -1}, {1, -1, 1}},
		// TOP_F
		{{-1, 1, -1}, {1, 1, -1}, {1, 1, 1}, {-1, 1, 1}},
		// BOTTOM_F
		{{1, -1, -1}, {-1, -1, -1}, {-1, -1, 1}, {1, -1, 1}}
	};

	for (int i = 0; i < 4; ++i) {
		BlockPosition cornerOffset = corners[static_cast<int>(face)][i];
		BlockPosition side1Pos, side2Pos, cornerPos;
		bool hasSide1, hasSide2, hasCorner;

		switch (face) {
		case FRONT_F:
			side1Pos = BlockPosition(blockPos.x + cornerOffset.x, blockPos.y + cornerOffset.y, blockPos.z);
			side2Pos = BlockPosition(blockPos.x, blockPos.y + cornerOffset.y, blockPos.z + cornerOffset.z);
			cornerPos = BlockPosition(blockPos.x + cornerOffset.x, blockPos.y + cornerOffset.y, blockPos.z + cornerOffset.z);
			break;
		case BACK_F:
			side1Pos = BlockPosition(blockPos.x + cornerOffset.x, blockPos.y + cornerOffset.y, blockPos.z);
			side2Pos = BlockPosition(blockPos.x, blockPos.y + cornerOffset.y, blockPos.z + cornerOffset.z);
			cornerPos = BlockPosition(blockPos.x + cornerOffset.x, blockPos.y + cornerOffset.y, blockPos.z + cornerOffset.z);
			break;
		case LEFT_F:
			side1Pos = BlockPosition(blockPos.x + cornerOffset.x, blockPos.y + cornerOffset.y, blockPos.z);
			side2Pos = BlockPosition(blockPos.x, blockPos.y + cornerOffset.y, blockPos.z + cornerOffset.z);
			cornerPos = BlockPosition(blockPos.x + cornerOffset.x, blockPos.y + cornerOffset.y, blockPos.z + cornerOffset.z);
			break;
		case RIGHT_F:
			side1Pos = BlockPosition(blockPos.x + cornerOffset.x, blockPos.y + cornerOffset.y, blockPos.z);
			side2Pos = BlockPosition(blockPos.x, blockPos.y + cornerOffset.y, blockPos.z + cornerOffset.z);
			cornerPos = BlockPosition(blockPos.x + cornerOffset.x, blockPos.y + cornerOffset.y, blockPos.z + cornerOffset.z);
			break;
		case TOP_F:
			side1Pos = BlockPosition(blockPos.x + cornerOffset.x, blockPos.y + cornerOffset.y, blockPos.z);
			side2Pos = BlockPosition(blockPos.x, blockPos.y + cornerOffset.y, blockPos.z + cornerOffset.z);
			cornerPos = BlockPosition(blockPos.x + cornerOffset.x, blockPos.y + cornerOffset.y, blockPos.z + cornerOffset.z);
			/*side1Pos = blockPos + cornerOffset;
			side2Pos = blockPos + cornerOffset;
			cornerPos = blockPos + cornerOffset;*/
			break;
		case BOTTOM_F:
			side1Pos = BlockPosition(blockPos.x + cornerOffset.x, blockPos.y + cornerOffset.y, blockPos.z);
			side2Pos = BlockPosition(blockPos.x, blockPos.y + cornerOffset.y, blockPos.z + cornerOffset.z);
			cornerPos = BlockPosition(blockPos.x + cornerOffset.x, blockPos.y + cornerOffset.y, blockPos.z + cornerOffset.z);
			break;
		}


		hasSide1 = getBlockGlobal(side1Pos) != BlockType::EMPTY; //isBlockSolid(side1Pos);
		hasSide2 = getBlockGlobal(side2Pos) != BlockType::EMPTY; //isBlockSolid(side2Pos);
		hasCorner = getBlockGlobal(cornerPos) != BlockType::EMPTY; //isBlockSolid(cornerPos);


		AOVals->push_back(vertexAO(hasSide1, hasSide2, hasCorner));
	}
}

uint8_t Chunk::vertexAO(bool s1, bool s2, bool corner) {
	if (s1 && s2) return 0;
	return 3 - (s1 + s2 + corner);
}

uint8_t Chunk::getBlockGlobal(const BlockPosition& pos) const {
	// If the position is inside this chunk, return it directly
	if (pos.x >= 0 && pos.x < chunkSize &&
		pos.y >= 0 && pos.y < chunkHeight &&
		pos.z >= 0 && pos.z < chunkSize) {
		return blocks.getBlock(pos);
	}

	// Calculate world-space block position
	glm::ivec3 worldPos = chunkPos + glm::vec3(pos.x, pos.y, pos.z);

	// Determine which chunk this position belongs to
	glm::ivec2 neighborChunkXZ(
		std::floor(worldPos.x / static_cast<float>(chunkSize)),
		std::floor(worldPos.z / static_cast<float>(chunkSize))
	);

	// Calculate local block position within the neighboring chunk
	BlockPosition localPos(
		(worldPos.x % chunkSize + chunkSize) % chunkSize,
		worldPos.y,
		(worldPos.z % chunkSize + chunkSize) % chunkSize
	);

	// Bounds check for Y (vertical)
	if (localPos.y < 0 || localPos.y >= chunkHeight) return BlockType::EMPTY;


	// Look up neighboring chunk
	if (loadedChunks && loadedChunks->count(neighborChunkXZ)) {
		const Chunk& neighbor = loadedChunks->at(neighborChunkXZ);
		return neighbor.blocks.getBlock(localPos);
	}

	return BlockType::EMPTY; // Default if chunk isn't loaded
}