#include <iostream>
#include "Block.h"
#include <glm/glm.hpp>

Block::Block() {
    this->position = glm::vec3(0, 0, 0); // Default position
    this->type = BlockData::BlockType::EMPTY; // Initialize type to EMPTY
    uMin = vMin = uMax = vMax = 0.0f; // Initialize UV coordinates to zero
    faceUVs = std::vector<glm::vec2>(4, glm::vec2(0, 0)); // Initialize face UVs to zero
}

Block::Block(glm::vec3 position, BlockData::BlockType type) : position(position), type(type) // Initialize faceUVs with 4 elements
{
    const std::tuple<int, int, int, int, int, int>& textureIndices = BlockData::textureIndices.at(type);
    faceUVs.resize(4);

    //UV coords for 0
    getUVFromAtlas(std::get<0>(textureIndices), 16, uMin, vMin, uMax, vMax);

    faceUVs[0] = glm::vec2(uMin, vMin); // Bottom Left
    faceUVs[1] = glm::vec2(uMax, vMin); // Bottom Right
    faceUVs[2] = glm::vec2(uMax, vMax); // Top Right
    faceUVs[3] = glm::vec2(uMin, vMax); // Top Left

    faces[BlockData::Faces(0)] = BlockData::FaceData{
        AddTransformedVertices(BlockData::rawVertexData.at(BlockData::Faces(0))),
        faceUVs
    };

    //UV coords for 1
    getUVFromAtlas(std::get<1>(textureIndices), 16, uMin, vMin, uMax, vMax);

    faceUVs[0] = glm::vec2(uMin, vMin); // Bottom Left
    faceUVs[1] = glm::vec2(uMax, vMin); // Bottom Right
    faceUVs[2] = glm::vec2(uMax, vMax); // Top Right
    faceUVs[3] = glm::vec2(uMin, vMax); // Top Left

    faces[BlockData::Faces(1)] = BlockData::FaceData{
        AddTransformedVertices(BlockData::rawVertexData.at(BlockData::Faces(1))),
        faceUVs
    };

    //UV coords for 2
    getUVFromAtlas(std::get<2>(textureIndices), 16, uMin, vMin, uMax, vMax);

    faceUVs[0] = glm::vec2(uMin, vMin); // Bottom Left
    faceUVs[1] = glm::vec2(uMax, vMin); // Bottom Right
    faceUVs[2] = glm::vec2(uMax, vMax); // Top Right
    faceUVs[3] = glm::vec2(uMin, vMax); // Top Left

    faces[BlockData::Faces(2)] = BlockData::FaceData{
        AddTransformedVertices(BlockData::rawVertexData.at(BlockData::Faces(2))),
        faceUVs
    };

    //UV coords for 3
    getUVFromAtlas(std::get<3>(textureIndices), 16, uMin, vMin, uMax, vMax);

    faceUVs[0] = glm::vec2(uMin, vMin); // Bottom Left
    faceUVs[1] = glm::vec2(uMax, vMin); // Bottom Right
    faceUVs[2] = glm::vec2(uMax, vMax); // Top Right
    faceUVs[3] = glm::vec2(uMin, vMax); // Top Left

    faces[BlockData::Faces(3)] = BlockData::FaceData{
        AddTransformedVertices(BlockData::rawVertexData.at(BlockData::Faces(3))),
        faceUVs
    };

    //UV coords for 4
    getUVFromAtlas(std::get<4>(textureIndices), 16, uMin, vMin, uMax, vMax);

    faceUVs[0] = glm::vec2(uMin, vMin); // Bottom Left
    faceUVs[1] = glm::vec2(uMax, vMin); // Bottom Right
    faceUVs[2] = glm::vec2(uMax, vMax); // Top Right
    faceUVs[3] = glm::vec2(uMin, vMax); // Top Left

    faces[BlockData::Faces(4)] = BlockData::FaceData{
        AddTransformedVertices(BlockData::rawVertexData.at(BlockData::Faces(4))),
        faceUVs
    };

    //UV coords for 5
    getUVFromAtlas(std::get<5>(textureIndices), 16, uMin, vMin, uMax, vMax);

    faceUVs[0] = glm::vec2(uMin, vMin); // Bottom Left
    faceUVs[1] = glm::vec2(uMax, vMin); // Bottom Right
    faceUVs[2] = glm::vec2(uMax, vMax); // Top Right
    faceUVs[3] = glm::vec2(uMin, vMax); // Top Left

    faces[BlockData::Faces(5)] = BlockData::FaceData{
        AddTransformedVertices(BlockData::rawVertexData.at(BlockData::Faces(5))),
        faceUVs
    };
}

std::vector<glm::vec3> Block::AddTransformedVertices(std::vector<glm::vec3> vertices)
{
    // Loop through each vertex and apply the transformation
    for (glm::vec3& vertex : vertices)
    {
        vertex += position;
    }

    return vertices;
}

BlockData::FaceData Block::GetFace(BlockData::Faces face)
{
    return faces[face];
}

void Block::getUVFromAtlas(int index, int atlasSize, float& uMin, float& vMin, float& uMax, float& vMax)
{
    int col = index % atlasSize;
    int row = index / atlasSize;

    float spriteSize = 1.0f / atlasSize;

    uMin = col * spriteSize;
    vMin = row * spriteSize;
    uMax = uMin + spriteSize;
    vMax = vMin + spriteSize;
}