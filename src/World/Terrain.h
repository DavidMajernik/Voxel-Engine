#pragma once
#include "World_Constants.h"
#include "../../LinkerStuff/dependencies/FastNoiseLite.h"


static FastNoiseLite noiseGenerator; // Noise generator for heightmap

class Terrain {

public:

	static void initializeNoiseGenerator() {
		noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin); // Set the noise type to Perlin noise
		noiseGenerator.SetFrequency(0.02f);

	}

	static std::array<std::array<int, chunkSize>, chunkSize> genHeightMap(float chunkPosX, float chunkPosZ) {
		
		std::array<std::array<int, chunkSize>, chunkSize> heightMap = std::array<std::array<int, chunkSize>, chunkSize>();
		float noiseValue;

		for (int x = 0; x < chunkSize; x++) {
			for (int z = 0; z < chunkSize; z++) {

				noiseValue = noiseGenerator.GetNoise(static_cast<float>(x + chunkPosX), static_cast<float>(z + chunkPosZ));
				noiseValue = (noiseValue + 1.0f) * 0.5f * chunkHeight + 1.0f;

				heightMap[x][z] = static_cast<int>(noiseValue);

			}
		}

		return heightMap;
	}

private: 

};