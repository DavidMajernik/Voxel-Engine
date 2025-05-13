#pragma once  
#include "World_Constants.h"  
#include "../../LinkerStuff/dependencies/FastNoiseLite.h"  
#include <memory> // Ensure std::unique_ptr is included  

class Terrain {  

public:  

    Terrain(int seed, float freq, int octaves, float lucunarity, float gain);

    std::array<std::array<int, (chunkSize + padding)>, (chunkSize + padding)> genHeightMap(float chunkPosX, float chunkPosZ);

private:  

   FastNoiseLite noiseGenerator;  

};