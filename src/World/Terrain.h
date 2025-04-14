#pragma once
#include "World_Constants.h"
#include "../../LinkerStuff/dependencies/FastNoiseLite.h"


static FastNoiseLite noiseGenerator; // Noise generator for heightmap
static FastNoiseLite noiseGenerator2; // Noise generator for heightmap
static FastNoiseLite noiseGenerator3; // Noise generator for heightmap
static FastNoiseLite noiseGenerator4; // Noise generator for heightmap


class Terrain {

public:

	static void initializeNoiseGenerator() {
		noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin); // Set the noise type to Perlin noise
		noiseGenerator.SetFrequency(0.02f);

		noiseGenerator2.SetNoiseType(FastNoiseLite::NoiseType_Perlin); // Set the noise type to Perlin noise
		noiseGenerator2.SetFrequency(0.1f);

		noiseGenerator3.SetNoiseType(FastNoiseLite::NoiseType_Perlin); // Set the noise type to Perlin noise
		noiseGenerator3.SetFrequency(0.2f);

		noiseGenerator4.SetNoiseType(FastNoiseLite::NoiseType_Perlin); // Set the noise type to Perlin noise
		noiseGenerator4.SetFrequency(0.3f);
	}

	static std::array<std::array<int, chunkSize>, chunkSize> genHeightMap(float chunkPosX, float chunkPosZ) {
		
		std::array<std::array<int, chunkSize>, chunkSize> heightMap = std::array<std::array<int, chunkSize>, chunkSize>();

		float noiseValue;
		float noiseValue2;
		float noiseValue3;
		float noiseValue4;
		float result;

		for (int x = 0; x < chunkSize; x++) {
			for (int z = 0; z < chunkSize; z++) {

				noiseValue = noiseGenerator.GetNoise(static_cast<float>(x + chunkPosX), static_cast<float>(z + chunkPosZ));
				noiseValue = (noiseValue + 1.0f) * 0.5f;

				noiseValue2 = noiseGenerator2.GetNoise(static_cast<float>(x + chunkPosX), static_cast<float>(z + chunkPosZ));
				noiseValue2 = (noiseValue2 + 1.0f) * 0.5f;

				noiseValue3 = noiseGenerator3.GetNoise(static_cast<float>(x + chunkPosX), static_cast<float>(z + chunkPosZ));
				noiseValue3 = (noiseValue3 + 1.0f) * 0.5f;

				noiseValue4 = noiseGenerator4.GetNoise(static_cast<float>(x + chunkPosX), static_cast<float>(z + chunkPosZ));
				noiseValue4 = (noiseValue4 + 1.0f) * 0.5f;

				result = (noiseValue * noiseValue2 * noiseValue3 * noiseValue4) * chunkHeight + 1.0f;

				heightMap[x][z] = static_cast<int>(result);

			}
		}

		return heightMap;
	}

private: 

};