#include "Chunk.h"

Texture* Chunk::texture = nullptr;

std::array<std::array<std::array<float, 4>, 6>, 256> Chunk::cachedUVs;

Chunk::Chunk() : chunkPos(glm::ivec3(0)), heightMap(){


}
Chunk::Chunk(glm::vec3 pos) : chunkPos(pos), blocks(), heightMap(){
 
	chunkMesh = Mesh();
	waterMesh = Mesh();


	Terrain terrain = Terrain(1337, 0.008f, 6, 2.0f, 0.25f);
	heightMap = terrain.genHeightMap(chunkPos.x, chunkPos.z);

	genBlocks(heightMap); 
	genFeatures(heightMap);


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
		return BlockType::EMPTY; 
	}
}

void Chunk::ChunkData::setBlock(const BlockPosition& blockPos, uint8_t blockType) {
	int index = getBlockIndex(blockPos);
	if (index >= 0 && index < blocks.size()) {
		blocks.at(index) = blockType;
	}
	else {
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
					if (y <= waterLevel + 3 && (y == columnHeight-1 || y == columnHeight - 2 || y == columnHeight - 3)) {
						// sand will be near the water level
						blocks.setBlock(BlockPosition(x, y, z), BlockType::SAND);

					}else if (y == columnHeight - 1) {
						//if above water level, top block is grass
						blocks.setBlock(BlockPosition(x, y, z), BlockType::GRASS);
					}
					else if (y == columnHeight - 2 || y == columnHeight - 3) {
						//next two blocks are dirt
						blocks.setBlock(BlockPosition(x, y, z), BlockType::DIRT);
					}
					else {
						//everything else is stone for now
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

void Chunk::genFeatures(std::array<std::array<int, (chunkSize + padding)>, (chunkSize + padding)>& heightMap)
{
	std::random_device rd; // rand number gen for tree placement. 
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> dist(1, 100);

	for (int x = 2; x < chunkSize; x++) {
		for (int z = 2; z < chunkSize; z++) {

			int columnHeight = heightMap[x][z];

			int rand = dist(rng);
			if (rand >= 99 && blocks.getBlock(BlockPosition(x, columnHeight-1, z)) == BlockType::GRASS) {
				//make a trunk
				blocks.setBlock(BlockPosition(x, columnHeight, z), BlockType::WOOD);
				blocks.setBlock(BlockPosition(x, columnHeight+1, z), BlockType::WOOD);
				blocks.setBlock(BlockPosition(x, columnHeight+2, z), BlockType::WOOD);
				blocks.setBlock(BlockPosition(x, columnHeight+3, z), BlockType::WOOD);
				blocks.setBlock(BlockPosition(x, columnHeight+4, z), BlockType::WOOD);
				//make the leaves
				blocks.setBlock(BlockPosition(x + 1, columnHeight + 3, z), BlockType::LEAVES);
				blocks.setBlock(BlockPosition(x, columnHeight + 3, z + 1), BlockType::LEAVES);
				blocks.setBlock(BlockPosition(x + 1, columnHeight + 3, z + 1), BlockType::LEAVES);
				blocks.setBlock(BlockPosition(x - 1, columnHeight + 3, z), BlockType::LEAVES);
				blocks.setBlock(BlockPosition(x, columnHeight + 3, z - 1), BlockType::LEAVES);
				blocks.setBlock(BlockPosition(x - 1, columnHeight + 3, z - 1), BlockType::LEAVES);

				blocks.setBlock(BlockPosition(x+1, columnHeight + 4, z), BlockType::LEAVES);
				blocks.setBlock(BlockPosition(x, columnHeight + 4, z+1), BlockType::LEAVES);
				blocks.setBlock(BlockPosition(x+1, columnHeight + 4, z+1), BlockType::LEAVES);
				blocks.setBlock(BlockPosition(x-1, columnHeight + 4, z), BlockType::LEAVES);
				blocks.setBlock(BlockPosition(x, columnHeight + 4, z-1), BlockType::LEAVES);
				blocks.setBlock(BlockPosition(x-1, columnHeight + 4, z-1), BlockType::LEAVES);

				blocks.setBlock(BlockPosition(x, columnHeight + 5, z), BlockType::LEAVES);

			}else if(rand <= 2 && blocks.getBlock(BlockPosition(x, columnHeight - 1, z)) == BlockType::GRASS) {
				blocks.setBlock(BlockPosition(x, columnHeight, z), BlockType::FLOWER_RED);
			}else if(rand > 2 && rand <= 3 && blocks.getBlock(BlockPosition(x, columnHeight - 1, z)) == BlockType::GRASS) {
			}else if (rand > 4 && rand <= 8 && blocks.getBlock(BlockPosition(x, columnHeight - 1, z)) == BlockType::GRASS) {
				blocks.setBlock(BlockPosition(x, columnHeight, z), BlockType::TALL_GRASS);
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

				//billboards
				if (blocks.getBlock(current) == BlockType::FLOWER_RED ||
					blocks.getBlock(current) == BlockType::TALL_GRASS) {
					addBillboard(current, blocks.getBlock(current));
					continue;
				}

				int numFaces = 0;

				//Front faces
				//qualifications for front face: Block to the front is empty
				if (isTransparent(BlockPosition(current.x, y, current.z + 1))) {
					integrateFace(current, Faces::FRONT_F);
					numFaces++;
				}

				//Back faces
				//qualifications for back face: Block to the back is empty
				if (isTransparent(BlockPosition(current.x, y, current.z - 1))) {
					integrateFace(current, Faces::BACK_F);
					numFaces++;

				}
					
				//Left faces
				//qualifications for left face: Block to the left is empty
				if (isTransparent(BlockPosition(current.x - 1, y, current.z))) {
					integrateFace(current, Faces::LEFT_F);
					numFaces++;

				}
				//Right faces
				//qualifications for right face: Block to the right is empty
				if (isTransparent(BlockPosition(current.x + 1, y, current.z))) {
					integrateFace(current, Faces::RIGHT_F);
					numFaces++;

				}
				//Top faces
				//qualifications for top face: Block to the top is empty, is farthest top in chunk. 
				if (y < chunkHeight - 1) {
					if (isTransparent(BlockPosition(current.x, y + 1, current.z))) {
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
					if (isTransparent(BlockPosition(current.x, y - 1, current.z))) {
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

			waterMesh.addVert(glm::vec3(static_cast<float>(vertex.x + blockPos.x + chunkPos.x),
				static_cast<float>(vertex.y + blockPos.y + chunkPos.y),
				static_cast<float>(vertex.z + blockPos.z + chunkPos.z)));

		}


		const auto& uv = cachedUVs[static_cast<int>(type)][face];

		waterMesh.addUV(glm::vec2(uv[0], uv[1])); // uMin, vMin
		waterMesh.addUV(glm::vec2(uv[2], uv[1])); // uMax, vMin
		waterMesh.addUV(glm::vec2(uv[2], uv[3])); // uMax, vMax
		waterMesh.addUV(glm::vec2(uv[0], uv[3])); // uMin, vMax
	}
	else {
		for (auto& vertex : rawVertexData.at(face)) {

			chunkMesh.addVert(glm::vec3(static_cast<float>(vertex.x + blockPos.x + chunkPos.x),
				static_cast<float>(vertex.y + blockPos.y + chunkPos.y),
				static_cast<float>(vertex.z + blockPos.z + chunkPos.z)));

		}


		const auto& uv = cachedUVs[static_cast<int>(type)][face];

		chunkMesh.addUV(glm::vec2(uv[0], uv[1])); // uMin, vMin
		chunkMesh.addUV(glm::vec2(uv[2], uv[1])); // uMax, vMin
		chunkMesh.addUV(glm::vec2(uv[2], uv[3])); // uMax, vMax
		chunkMesh.addUV(glm::vec2(uv[0], uv[3])); // uMin, vMax
	}

	glm::vec3 normal{};
	
	//Add normals based on face
	switch (face) {
	case FRONT_F:  normal = glm::vec3(0, 0, 1); break;
	case BACK_F:   normal = glm::vec3(0, 0, -1); break;
	case LEFT_F:   normal = glm::vec3(-1, 0, 0); break;
	case RIGHT_F:  normal = glm::vec3(1, 0, 0); break;
	case TOP_F:    normal = glm::vec3(0, 1, 0); break;
	case BOTTOM_F: normal = glm::vec3(0, -1, 0); break;
	}
	//Add normals to the vector, per vertex.
	for(int i = 0; i < 4; i++) {
		if (water) {
			waterMesh.addNormal(normal);
		}
		else {
			chunkMesh.addNormal(normal);
		}
	}


	//add ao vals
	generateAOVals(blockPos, face, water);

}


void Chunk::addIndices(int amtFaces, bool water)
{
	if (water) {
		waterMesh.addIndices(amtFaces);
	}
	else {
		chunkMesh.addIndices(amtFaces);
	}
}

void Chunk::uploadToGPU()
{
	// normal blocks
	chunkMesh.uploadToGPU();

	//billboards
	billboardMesh.uploadToGPU();

	//Water
	waterMesh.uploadToGPU();
}


void Chunk::renderSolids(Shader& shader)
{
	//normal blocks
	shader.use(); 
	texture->Bind(GL_TEXTURE0);

	chunkMesh.render(false);
}

void Chunk::renderWater(Shader& waterShader)
{
	//water blocks
	waterShader.use();
	texture->Bind(GL_TEXTURE0);

	waterMesh.render(true);

}

void Chunk::renderBillboards(Shader& shader)
{
	//billboards
	shader.use();
	texture->Bind(GL_TEXTURE0);

	billboardMesh.render(true);
}

void Chunk::initializeTexture() {
	if (texture == nullptr) {
		texture = new Texture(GL_TEXTURE_2D, "assets/MinecraftAtlas3.png");
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
	chunkMesh.Delete();
	waterMesh.Delete();
	billboardMesh.Delete();

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
			waterMesh.addAOVal(vertexAO(hasSide1, hasSide2, hasCorner));
		}else{
			chunkMesh.addAOVal(vertexAO(hasSide1, hasSide2, hasCorner));
		}
	}
}

uint8_t Chunk::vertexAO(bool s1, bool s2, bool corner) {
	if (s1 && s2) return 0;
	return 3 - (s1 + s2 + corner);
}

void Chunk::addBillboard(const BlockPosition& pos, uint8_t type) {
	// Center the quad at the block position
	glm::vec3 center = glm::vec3(pos.x + chunkPos.x, pos.y + chunkPos.y, pos.z + chunkPos.z);

	float halfSize = 0.5f;
	std::vector<glm::vec3> quadVertsX = {
		center + glm::vec3(halfSize, halfSize, 0.0f),
		center + glm::vec3(-halfSize, halfSize, 0.0f),
		center + glm::vec3(-halfSize, -halfSize, 0.0f),
		center + glm::vec3(halfSize, -halfSize, 0.0f),
	};

	std::vector<glm::vec3> quadVertsZ = {
		center + glm::vec3(0.0f, halfSize, -halfSize),
		center + glm::vec3(0.0f, halfSize, halfSize),
		center + glm::vec3(0.0f, -halfSize, halfSize),
		center + glm::vec3(0.0f, -halfSize, -halfSize),
	};

	const auto& uv = cachedUVs[static_cast<int>(type)][0];

	for (const auto& quad : { quadVertsX, quadVertsZ }) {
		for (const auto& vertex : quad) {
			billboardMesh.addVert(vertex);

			billboardMesh.addUV(glm::vec2(uv[0], uv[1]));
			billboardMesh.addUV(glm::vec2(uv[2], uv[1]));
			billboardMesh.addUV(glm::vec2(uv[2], uv[3]));
			billboardMesh.addUV(glm::vec2(uv[0], uv[3]));

			for (int i = 0; i < 4; ++i)
				billboardMesh.addNormal(glm::vec3(0, 1, 0));

			billboardMesh.addIndices(1);

			for (int i = 0; i < 4; ++i)
				billboardMesh.addAOVal(3);
		}
	}
	
}

bool Chunk::isTransparent(const BlockPosition& blockPos) const {
	uint8_t blockType = blocks.getBlock(blockPos);

	bool isTransparent = (blockType == BlockType::EMPTY ||
						  blockType == BlockType::WATER || 
						  blockType == BlockType::LEAVES ||
						  blockType == BlockType::FLOWER_RED ||
						  blockType == BlockType::TALL_GRASS);

	return isTransparent;
}
