#include <iostream>
#include <glm/glm.hpp>
#include "BlockData.h"

const std::map<BlockData::Faces, std::vector<glm::vec3>> BlockData::rawVertexData = 
{
    {BlockData::Faces::FRONT, {
            glm::vec3(-0.5f,  0.5f,  0.5f), //Top Left
            glm::vec3(0.5f,  0.5f,  0.5f), //Top Right
            glm::vec3(0.5f, -0.5f,  0.5f), //Bottom Right
            glm::vec3(-0.5f, -0.5f,  0.5f)  //Bottom Left
        }
    },
    {
        BlockData::Faces::BACK, {
            glm::vec3(0.5f,  0.5f, -0.5f),
            glm::vec3(-0.5f,  0.5f, -0.5f),
            glm::vec3(-0.5f, -0.5f, -0.5f),
            glm::vec3(0.5f, -0.5f, -0.5f)
        }
    },
    {
        BlockData::Faces::LEFT, {
            glm::vec3(-0.5f,  0.5f, -0.5f),
            glm::vec3(-0.5f,  0.5f,  0.5f),
            glm::vec3(-0.5f, -0.5f,  0.5f),
            glm::vec3(-0.5f, -0.5f, -0.5f)
        }
    },
    {
        BlockData::Faces::RIGHT, {
            glm::vec3(0.5f,  0.5f,  0.5f),
            glm::vec3(0.5f,  0.5f, -0.5f),
            glm::vec3(0.5f, -0.5f, -0.5f),
            glm::vec3(0.5f, -0.5f,  0.5f)
        }
    },
    {
        BlockData::Faces::TOP, {
            glm::vec3(-0.5f,  0.5f, -0.5f),
            glm::vec3(0.5f,  0.5f, -0.5f),
            glm::vec3(0.5f,  0.5f,  0.5f),
            glm::vec3(-0.5f,  0.5f,  0.5f)
        }
    },
    {
        BlockData::Faces::BOTTOM, {
            glm::vec3(-0.5f, -0.5f,  0.5f),
            glm::vec3(0.5f, -0.5f,  0.5f),
            glm::vec3(0.5f, -0.5f, -0.5f),
            glm::vec3(-0.5f, -0.5f, -0.5f)
        }
    }
};

const std::map<BlockData::BlockType, std::tuple<int, int, int, int, int, int>> BlockData::textureIndices =
{
    {BlockData::BlockType::GRASS, std::make_tuple(3, 3, 3, 3, 0, 2) }, // Grass Texture
    { BlockData::BlockType::DIRT,  std::make_tuple(2, 2, 2, 2, 2, 2) }, // Dirt Texture
    { BlockData::BlockType::SAND,   std::make_tuple(176, 176, 176, 176, 176, 176) }, // Sand Texture
    { BlockData::BlockType::STONE,  std::make_tuple(1, 1, 1, 1, 1, 1) },  // Stone Texture
    { BlockData::BlockType::EMPTY,  std::make_tuple(0, 0, 0, 0, 0, 0) } // Empty Texture (no texture)
    
};

BlockData::BlockData()
{

}