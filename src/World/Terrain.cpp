#include "Terrain.h"

Terrain::Terrain(int seed, float freq, int octaves, float lucunarity, float gain) {

    noiseGenerator = FastNoiseLite(seed);
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noiseGenerator.SetFractalType(FastNoiseLite::FractalType_FBm);
    noiseGenerator.SetFrequency(freq);
    noiseGenerator.SetFractalOctaves(octaves);
    noiseGenerator.SetFractalLacunarity(lucunarity);
    noiseGenerator.SetFractalGain(gain);

}

std::array<std::array<int, (chunkSize + padding)>, (chunkSize + padding)> Terrain::genHeightMap(float chunkPosX, float chunkPosZ) {

    std::array<std::array<int, (chunkSize + padding)>, (chunkSize + padding)> heightMap = std::array<std::array<int, (chunkSize + padding)>, (chunkSize + padding)>();

    float noiseValue, noiseValue2;

    for (int x = 0; x < (chunkSize + padding); x++) {
        for (int z = 0; z < (chunkSize + padding); z++) {

            noiseValue = noiseGenerator.GetNoise(static_cast<float>(x + chunkPosX), static_cast<float>(z + chunkPosZ));
            
            noiseValue = (noiseValue + 1.0f) * 0.5f;

            noiseValue = (noiseValue)*256 + 1.0f;

            heightMap[x][z] = static_cast<int>(noiseValue);

        }
    }

    return heightMap;
}