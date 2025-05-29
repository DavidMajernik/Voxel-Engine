#include "Chunk.h"

Texture* Chunk::texture = nullptr; // Initialize the static texture pointer to nullptr

std::array<std::array<std::array<float, 4>, 6>, 256> Chunk::cachedUVs;

Chunk::Chunk() : chunkPos(glm::ivec3(0)), indexCount(0), chunkVAO(0), chunkVertexVBO(0), chunkUVVBO(0), chunkEBO(0), chunkAOBO(0), heightMap(),
waterVAO(0), waterVertexVBO(0), waterUVVBO(0), waterEBO(0), waterAOBO(0), waterIndexCount(0) {


}
Chunk::Chunk(glm::vec3 pos) : chunkPos(pos), indexCount(0), chunkVAO(0), chunkVertexVBO(0), chunkUVVBO(0), chunkEBO(0), blocks(), chunkAOBO(0), heightMap(),
waterVAO(0), waterVertexVBO(0), waterUVVBO(0), waterEBO(0), waterAOBO(0), waterIndexCount(0) {

	chunkVerts = std::make_unique<std::vector<glm::vec3>>();
	chunkUVs = std::make_unique<std::vector<glm::vec2>>();
	chunkIndices = std::make_unique<std::vector<unsigned int>>();
	AOVals = std::make_unique<std::vector<uint8_t>>();

	waterVerts = std::make_unique<std::vector<glm::vec3>>();
	waterUVs = std::make_unique<std::vector<glm::vec2>>();
	waterIndices = std::make_unique<std::vector<unsigned int>>();
	waterAOVals = std::make_unique<std::vector<uint8_t>>();

	Terrain terrain = Terrain(1337, 0.008f, 6, 2.0f, 0.25f);
	heightMap = terrain.genHeightMap(chunkPos.x, chunkPos.z); 

	genBlocks(heightMap); 


}

inline int getBlockIndex(const BlockPosition& blockPos) {
	return  blockPos.x + (blockPos.y * (chunkSize + padding)) + (blockPos.z * (chunkSize + padding) * chunkHeight);
}


uint8_t Chunk::ChunkData::getBlock(const BlockPosition& blockPos) const {
	int index = getBlockIndex(blockPos);
	if (index >= 0 && index < blocks.size()) {
		return blocks.at(index);
	}
	else {
		//std::cerr << "GetBlock out of bounds: " << index << std::endl;
		return BlockType::EMPTY; 
	}
}

void Chunk::ChunkData::setBlock(const BlockPosition& blockPos, uint8_t blockType) {
	int index = getBlockIndex(blockPos);
	if (index >= 0 && index < blocks.size()) {
		blocks.at(index) = blockType;
	}
	else {
		//std::cerr << "SetBlock out of bounds: " << index << std::endl;
	}
}

uint8_t Chunk::getBlock(const BlockPosition& blockPos) const {
	return blocks.getBlock(blockPos);
}

void Chunk::setBlock(const BlockPosition& blockPos, uint8_t blockType) {
	blocks.setBlock(blockPos, blockType);
}

void Chunk::genBlocks(std::array<std::array<int, (chunkSize + padding)>, (chunkSize + padding)> &heightMap)
{
	for (int x = 0; x < (chunkSize + padding); x++) {
		for (int z = 0; z < (chunkSize + padding); z++) {

			int columnHeight = heightMap[x][z];

			for (int y = 0; y < chunkHeight; y++) {

				
				if (y < columnHeight) {
					if ((y == columnHeight - 1)) {
						blocks.setBlock(BlockPosition(x, y, z), BlockType::GRASS);
					}
					else if (y == columnHeight - 2 || y == columnHeight - 3) {
						blocks.setBlock(BlockPosition(x, y, z), BlockType::DIRT);
					}
					else {
						blocks.setBlock(BlockPosition(x, y, z), BlockType::STONE);
					}
				}
				else if (y >= columnHeight && y <= waterLevel) {
					blocks.setBlock(BlockPosition(x, y, z), BlockType::WATER);
				}
				else {
					blocks.setBlock(BlockPosition(x, y, z), BlockType::EMPTY);
				}
			}
		}
	}

}

void Chunk::genFaces() {
	
	for (int x = 0; x < chunkSize; x++) {
		for (int z = 0; z < chunkSize; z++) {

			for (int y = 0; y < chunkHeight; y++) {

				
				BlockPosition current = BlockPosition(x+1, y, z+1);
				
				if (blocks.getBlock(current) == BlockType::EMPTY) continue;

				//we only need the top face of water blocks
				if (blocks.getBlock(current) == BlockType::WATER) {
						if (y < chunkHeight - 1 && blocks.getBlock(BlockPosition(current.x, y + 1, current.z)) == BlockType::EMPTY) {
							integrateFace(current, Faces::TOP_F);
							addIndices(1, true);
						}
					continue;
				}

				int numFaces = 0;

				//Front faces
				//qualifications for front face: Block to the front is empty
				if (blocks.getBlock(BlockPosition(current.x, y, current.z + 1)) == BlockType::EMPTY ||
					blocks.getBlock(BlockPosition(current.x, y, current.z + 1)) == BlockType::WATER) {
					integrateFace(current, Faces::FRONT_F);
					numFaces++;
				}

				//Back faces
				//qualifications for back face: Block to the back is empty
				if (blocks.getBlock(BlockPosition(current.x, y, current.z - 1)) == BlockType::EMPTY ||
					blocks.getBlock(BlockPosition(current.x, y, current.z - 1)) == BlockType::WATER) {
					integrateFace(current, Faces::BACK_F);
					numFaces++;

				}
					
				//Left faces
				//qualifications for left face: Block to the left is empty
				if (blocks.getBlock(BlockPosition(current.x - 1, y, current.z)) == BlockType::EMPTY ||
					blocks.getBlock(BlockPosition(current.x - 1, y, current.z)) == BlockType::WATER) {
					integrateFace(current, Faces::LEFT_F);
					numFaces++;

				}
				//Right faces
				//qualifications for right face: Block to the right is empty
				if (blocks.getBlock(BlockPosition(current.x + 1, y, current.z)) == BlockType::EMPTY ||
					blocks.getBlock(BlockPosition(current.x + 1, y, current.z)) == BlockType::WATER) {
					integrateFace(current, Faces::RIGHT_F);
					numFaces++;

				}
				//Top faces
				//qualifications for top face: Block to the top is empty, is farthest top in chunk. 
				if (y < chunkHeight - 1) {
					if (blocks.getBlock(BlockPosition(current.x, y + 1, current.z)) == BlockType::EMPTY ||
						blocks.getBlock(BlockPosition(current.x, y + 1, current.z)) == BlockType::WATER) {
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
					if (blocks.getBlock(BlockPosition(current.x, y - 1, current.z)) == BlockType::EMPTY ||
						blocks.getBlock(BlockPosition(current.x, y - 1, current.z)) == BlockType::WATER) {
						integrateFace(current, Faces::BOTTOM_F);
						numFaces++;

					}
				}

			addIndices(numFaces, false);

			}

		}
	}

}

void Chunk::integrateFace(BlockPosition blockPos, Faces face) {

	BlockType type = static_cast<BlockType>(blocks.getBlock(blockPos));
	bool water = false;

	if (type == BlockType::WATER) {
		water = true;
		for (auto& vertex : rawVertexData.at(face)) {

			waterVerts->push_back(glm::vec3(static_cast<float>(vertex.x + blockPos.x + chunkPos.x),
				static_cast<float>(vertex.y + blockPos.y + chunkPos.y),
				static_cast<float>(vertex.z + blockPos.z + chunkPos.z)));

		}


		const auto& uv = cachedUVs[static_cast<int>(type)][face];

		waterUVs->emplace_back(uv[0], uv[1]); // uMin, vMin
		waterUVs->emplace_back(uv[2], uv[1]); // uMax, vMin
		waterUVs->emplace_back(uv[2], uv[3]); // uMax, vMax
		waterUVs->emplace_back(uv[0], uv[3]); // uMin, vMax
	}
	else {
		for (auto& vertex : rawVertexData.at(face)) {

			chunkVerts->push_back(glm::vec3(static_cast<float>(vertex.x + blockPos.x + chunkPos.x),
				static_cast<float>(vertex.y + blockPos.y + chunkPos.y),
				static_cast<float>(vertex.z + blockPos.z + chunkPos.z)));

		}


		const auto& uv = cachedUVs[static_cast<int>(type)][face];

		chunkUVs->emplace_back(uv[0], uv[1]); // uMin, vMin
		chunkUVs->emplace_back(uv[2], uv[1]); // uMax, vMin
		chunkUVs->emplace_back(uv[2], uv[3]); // uMax, vMax
		chunkUVs->emplace_back(uv[0], uv[3]); // uMin, vMax
	}

	//add ao vals
	generateAOVals(blockPos, face, water);

}


void Chunk::addIndices(int amtFaces, bool water)
{
	if (water) {
		for (int i = 0; i < amtFaces; i++) {
			waterIndices->push_back(0 + waterIndexCount);
			waterIndices->push_back(1 + waterIndexCount);
			waterIndices->push_back(2 + waterIndexCount);
			waterIndices->push_back(2 + waterIndexCount);
			waterIndices->push_back(3 + waterIndexCount);
			waterIndices->push_back(0 + waterIndexCount);

			waterIndexCount += 4;
		}
	}
	else {
		for (int i = 0; i < amtFaces; i++) {
			chunkIndices->push_back(0 + indexCount);
			chunkIndices->push_back(1 + indexCount);
			chunkIndices->push_back(2 + indexCount);
			chunkIndices->push_back(2 + indexCount);
			chunkIndices->push_back(3 + indexCount);
			chunkIndices->push_back(0 + indexCount);

			indexCount += 4;
		}
	}
}

void Chunk::uploadToGPU()
{
	// normal blocks
	glGenVertexArrays(1, &chunkVAO); 
	glBindVertexArray(chunkVAO); 


	glGenBuffers(1, &chunkVertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, chunkVertexVBO); 
	glBufferData(GL_ARRAY_BUFFER, chunkVerts->size() * sizeof(glm::vec3), chunkVerts->data(), GL_STATIC_DRAW); 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); 
	glEnableVertexAttribArray(0); 

	glGenBuffers(1, &chunkUVVBO);
	glBindBuffer(GL_ARRAY_BUFFER, chunkUVVBO); 
	glBufferData(GL_ARRAY_BUFFER, chunkUVs->size() * sizeof(glm::vec2), chunkUVs->data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &chunkEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunkEBO); 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunkIndices->size() * sizeof(unsigned int), chunkIndices->data(), GL_STATIC_DRAW);

	glGenBuffers(1, &chunkAOBO);
	glBindBuffer(GL_ARRAY_BUFFER, chunkAOBO);
	glBufferData(GL_ARRAY_BUFFER, AOVals->size() * sizeof(uint8_t), AOVals->data(), GL_STATIC_DRAW);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(uint8_t), (void*)0); 
	glEnableVertexAttribArray(2);

	//Water
	glGenVertexArrays(1, &waterVAO);
	glBindVertexArray(waterVAO);


	glGenBuffers(1, &waterVertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, waterVertexVBO);
	glBufferData(GL_ARRAY_BUFFER, waterVerts->size() * sizeof(glm::vec3), waterVerts->data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &waterUVVBO);
	glBindBuffer(GL_ARRAY_BUFFER, waterUVVBO);
	glBufferData(GL_ARRAY_BUFFER, waterUVs->size() * sizeof(glm::vec2), waterUVs->data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &waterEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, waterIndices->size() * sizeof(unsigned int), waterIndices->data(), GL_STATIC_DRAW);

	glGenBuffers(1, &waterAOBO);
	glBindBuffer(GL_ARRAY_BUFFER, waterAOBO);
	glBufferData(GL_ARRAY_BUFFER, waterAOVals->size() * sizeof(uint8_t), waterAOVals->data(), GL_STATIC_DRAW);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(uint8_t), (void*)0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}


void Chunk::renderSolids(Shader& shader)
{
	//normal blocks
	shader.use(); 
	glBindVertexArray(chunkVAO);
	texture->Bind(GL_TEXTURE0); 
	glDrawElements(GL_TRIANGLES, chunkIndices->size(), GL_UNSIGNED_INT, 0); 
	glBindVertexArray(0);

	GLenum error;
	if ((error = glGetError()) != GL_NO_ERROR) {
		std::cerr << "Solids OpenGL Error: " << error << std::endl;
	}
}

void Chunk::renderWater(Shader& waterShader)
{
	//water blocks
	waterShader.use();
	glBindVertexArray(waterVAO);
	texture->Bind(GL_TEXTURE0);

	glDisable(GL_CULL_FACE);
	glDrawElements(GL_TRIANGLES, waterIndices->size(), GL_UNSIGNED_INT, 0);
	glEnable(GL_CULL_FACE);

	glBindVertexArray(0);

	GLenum error;
	if ((error = glGetError()) != GL_NO_ERROR) {
		std::cerr << "Water OpenGL Error: " << error << std::endl;
	}
}

void Chunk::initializeTexture() {
	if (texture == nullptr) {
		texture = new Texture(GL_TEXTURE_2D, "assets/MinecraftAtlas.png");
		texture->Load();
	}
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

	glDeleteVertexArrays(1, &waterVAO);
	glDeleteBuffers(1, &waterVertexVBO);
	glDeleteBuffers(1, &waterUVVBO);
	glDeleteBuffers(1, &waterEBO);
	glDeleteBuffers(1, &waterAOBO);

	waterIndexCount = 0;
	waterVerts->clear();
	waterUVs->clear();
	waterIndices->clear();
	waterAOVals->clear();

}

void Chunk::cacheUVsFromAtlas() {
	int atlasSize = 16; 
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

void Chunk::generateAOVals(BlockPosition blockPos, Faces face, bool water) {

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
			break;
		case BOTTOM_F:
			side1Pos = BlockPosition(blockPos.x + cornerOffset.x, blockPos.y + cornerOffset.y, blockPos.z);
			side2Pos = BlockPosition(blockPos.x, blockPos.y + cornerOffset.y, blockPos.z + cornerOffset.z);
			cornerPos = BlockPosition(blockPos.x + cornerOffset.x, blockPos.y + cornerOffset.y, blockPos.z + cornerOffset.z);
			break;
		}


		hasSide1 = blocks.getBlock(side1Pos) != BlockType::EMPTY;
		hasSide2 = blocks.getBlock(side2Pos) != BlockType::EMPTY;
		hasCorner = blocks.getBlock(cornerPos) != BlockType::EMPTY;

		if (water) {
			waterAOVals->push_back(vertexAO(hasSide1, hasSide2, hasCorner));
		}else{
			AOVals->push_back(vertexAO(hasSide1, hasSide2, hasCorner));
		}
	}
}

uint8_t Chunk::vertexAO(bool s1, bool s2, bool corner) {
	if (s1 && s2) return 0;
	return 3 - (s1 + s2 + corner);
}
