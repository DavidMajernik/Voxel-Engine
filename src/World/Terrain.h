#pragma once
#include "World_Constants.h"
#include "../../LinkerStuff/dependencies/FastNoiseLite.h"

class Terrain {

public:

    static void initializeNoiseGenerator(int seed, float freq, int octaves, float lucunarity, float gain) {

        noiseGenerator = FastNoiseLite(seed);
        noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin); 
        noiseGenerator.SetFractalType(FastNoiseLite::FractalType_FBm);
        noiseGenerator.SetFrequency(freq);
        noiseGenerator.SetFractalOctaves(octaves);
        noiseGenerator.SetFractalLacunarity(lucunarity);
        noiseGenerator.SetFractalGain(gain);
    }

    static std::array<std::array<int, chunkSize>, chunkSize> genHeightMap(float chunkPosX, float chunkPosZ) {
        
        std::array<std::array<int, chunkSize>, chunkSize> heightMap = std::array<std::array<int, chunkSize>, chunkSize>();

        float noiseValue;

        for (int x = 0; x < chunkSize; x++) {
            for (int z = 0; z < chunkSize; z++) {

                noiseValue = noiseGenerator.GetNoise(static_cast<float>(x + chunkPosX), static_cast<float>(z + chunkPosZ));
                noiseValue = (noiseValue + 1.0f) * 0.5f;

                noiseValue = (noiseValue) * chunkHeight + 1.0f;    

                heightMap[x][z] = static_cast<int>(noiseValue);

            }
        }

        return heightMap;
    }

private: 
	static FastNoiseLite noiseGenerator;
};