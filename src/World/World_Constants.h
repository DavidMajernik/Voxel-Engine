#pragma once
#include <cstdint>
#include <unordered_map>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

const uint8_t chunkSize = 32;
const uint8_t chunkHeight = 32; // Maximum height of the chunk
const int chunkArea = chunkSize * chunkSize;
const int chunkVolume = chunkArea * chunkHeight;
static const int renderDistance = 2; // The distance in chunks to render around the player

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

const std::unordered_map<BlockType, std::tuple<int, int, int, int, int, int>> textureIndices =
{
    {BlockType::GRASS, std::make_tuple(3, 3, 3, 3, 0, 2) }, // Grass Texture
    {BlockType::DIRT,  std::make_tuple(2, 2, 2, 2, 2, 2) }, // Dirt Texture
    {BlockType::SAND,   std::make_tuple(176, 176, 176, 176, 176, 176) }, // Sand Texture
    {BlockType::STONE,  std::make_tuple(1, 1, 1, 1, 1, 1) },  // Stone Texture

};