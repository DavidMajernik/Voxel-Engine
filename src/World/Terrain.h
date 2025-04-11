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

	static int getNoiseAt(float x, float z) {
		float noiseValue = noiseGenerator.GetNoise(static_cast<float>(x), static_cast<float>(z));
		noiseValue = (noiseValue + 1.0f) * 0.5f * chunkHeight;
		
		return static_cast<int>(noiseValue);
	}

private: 

	Terrain() = default; // Private constructor 

};