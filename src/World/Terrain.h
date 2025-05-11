#pragma once  
#include "World_Constants.h"  
#include "../../LinkerStuff/dependencies/FastNoiseLite.h"  
#include <memory> // Ensure std::unique_ptr is included  

class Terrain {  

public:  

    Terrain(int seed, float freq, int octaves, float lucunarity, float gain);

    std::array<std::array<int, (chunkSize + padding)>, (chunkSize + padding)> genHeightMap(float chunkPosX, float chunkPosZ);

   //static std::unique_ptr<uint8_t[]> genCaves(std::array<std::array<int, chunkSize>, chunkSize>& heightMap, float chunkPosX, float chunkPosZ) {  
   //    auto caveMap = std::make_unique<uint8_t[]>(chunkVolume); // Use std::make_unique for heap allocation  
   //    float noiseValue;  

   //    for (int x = 0; x < chunkSize; x++) {  
   //        for (int y = 0; y < chunkHeight; y++) {  
   //            for (int z = 0; z < chunkSize; z++) {  
   //                noiseValue = caveNoiseGenerator.GetNoise(static_cast<float>(x + chunkPosX), static_cast<float>(y), static_cast<float>(z + chunkPosZ));  
   //                noiseValue = (noiseValue + 1.0f) * 0.5f;  
   //                size_t index = x + y * chunkSize + z * chunkSize * chunkHeight;  
   //                if (noiseValue > 0.5f) {  
   //                    caveMap[index] = BlockType::EMPTY;  
   //                }  
   //                else {  
   //                    caveMap[index] = BlockType::STONE;  
   //                }  
   //            }  
   //        }  
   //    }  

   //    return caveMap;  
   //}  

private:  

   FastNoiseLite noiseGenerator;  
   //static FastNoiseLite caveNoiseGenerator;  

};