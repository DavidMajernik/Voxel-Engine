#pragma once
#include <cstdint>
#include <unordered_map>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <array>

constexpr uint8_t chunkSize = 32;
constexpr uint8_t padding = 2;
constexpr int chunkHeight = 256; // Maximum height of the chunk
constexpr int chunkArea = (chunkSize + padding) * (chunkSize + padding);
constexpr int chunkVolume = chunkArea * chunkHeight;
constexpr int renderDistance = 10; // The distance in chunks to render around the player
constexpr int unloadDistance = renderDistance + 1;
constexpr int waterLevel = 80; // The level at which water is generated


enum BlockType : uint8_t {
	EMPTY,
	DIRT, 
	GRASS,
    STONE,
    SAND,
    WATER
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
	{BlockType::WATER, {205, 205, 205, 205, 205, 205} },             // Water Texture

};

const std::vector<glm::vec2> neighborOffsets = {
    {1, 0}, {-1, 0}, {0, 1}, {0, -1}
};