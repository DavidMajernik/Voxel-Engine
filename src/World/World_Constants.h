#pragma once
#include <cstdint>
#include <unordered_map>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <array>

const uint8_t chunkSize = 32;
const uint8_t chunkHeight = 32; // Maximum height of the chunk
const int chunkArea = chunkSize * chunkSize;
const int chunkVolume = chunkArea * chunkHeight;
static const int renderDistance = 3; // The distance in chunks to render around the player
static const int unloadDistance = renderDistance + 1;

enum BlockType : uint8_t {
	EMPTY,
	DIRT, 
	GRASS,
    STONE,
    SAND
};

enum Faces : uint8_t {
	FRONT_F,
	BACK_F,
	LEFT_F,
	RIGHT_F,
	TOP_F,
	BOTTOM_F
};

const std::unordered_map<Faces, std::vector<glm::vec3>> rawVertexData =
{
    {Faces::FRONT_F, {
            glm::vec3(-0.5f,  0.5f,  0.5f), //Top Left
            glm::vec3(0.5f,  0.5f,  0.5f), //Top Right
            glm::vec3(0.5f, -0.5f,  0.5f), //Bottom Right
            glm::vec3(-0.5f, -0.5f,  0.5f)  //Bottom Left
        }
    },
    {
        Faces::BACK_F, {
            glm::vec3(0.5f,  0.5f, -0.5f),
            glm::vec3(-0.5f,  0.5f, -0.5f),
            glm::vec3(-0.5f, -0.5f, -0.5f),
            glm::vec3(0.5f, -0.5f, -0.5f)
        }
    },
    {
        Faces::LEFT_F, {
            glm::vec3(-0.5f,  0.5f, -0.5f),
            glm::vec3(-0.5f,  0.5f,  0.5f),
            glm::vec3(-0.5f, -0.5f,  0.5f),
            glm::vec3(-0.5f, -0.5f, -0.5f)
        }
    },
    {
        Faces::RIGHT_F, {
            glm::vec3(0.5f,  0.5f,  0.5f),
            glm::vec3(0.5f,  0.5f, -0.5f),
            glm::vec3(0.5f, -0.5f, -0.5f),
            glm::vec3(0.5f, -0.5f,  0.5f)
        }
    },
    {
        Faces::TOP_F, {
            glm::vec3(-0.5f,  0.5f, -0.5f),
            glm::vec3(0.5f,  0.5f, -0.5f),
            glm::vec3(0.5f,  0.5f,  0.5f),
            glm::vec3(-0.5f,  0.5f,  0.5f)
        }
    },
    {
        Faces::BOTTOM_F, {
            glm::vec3(-0.5f, -0.5f,  0.5f),
            glm::vec3(0.5f, -0.5f,  0.5f),
            glm::vec3(0.5f, -0.5f, -0.5f),
            glm::vec3(-0.5f, -0.5f, -0.5f)
        }
    }
};

const std::unordered_map<BlockType, std::array<int, 6>> textureIndices =
{
    {BlockType::GRASS, {3, 3, 3, 3, 0, 2} },             // Grass Texture
    {BlockType::DIRT,  {2, 2, 2, 2, 2, 2} },             // Dirt Texture
    {BlockType::SAND,  {176, 176, 176, 176, 176, 176} }, // Sand Texture
    {BlockType::STONE, {1, 1, 1, 1, 1, 1} },             // Stone Texture

};

const std::vector<glm::vec2> neighborOffsets = {
    {1, 0}, {-1, 0}, {0, 1}, {0, -1}
};