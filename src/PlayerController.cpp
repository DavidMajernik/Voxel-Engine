#include "PlayerController.h"

PlayerController::PlayerController() {}

PlayerController::~PlayerController() {}

void PlayerController::RayCast(glm::vec3 camPos, glm::vec3 direction, std::unique_ptr<World>& world, bool place) {
    // Pseudocode:
    // 1. Step along the ray in small increments (e.g., 0.1 units).
    // 2. For each step, check if the block at the current position is not air (block != 0).
    // 3. If place is true, set the block just before the hit to dirt (blockType = 1).
    //    If place is false, set the hit block to air (blockType = 0).
    // 4. Stop after the first hit or after a max distance.

    const float maxDistance = 6.0f;
    const float step = 0.01f;
    glm::vec3 pos = camPos;
    glm::vec3 prevPos = pos;
    direction = glm::normalize(direction);

    for (float t = 0.0f; t < maxDistance; t += step) {
        glm::ivec3 blockPos = glm::ivec3(glm::floor(pos));
        uint8_t block = world->getBlockGlobal(glm::vec3(blockPos));
        if (block != 0) {
            if (place) {
                // Place block at previous position (if not inside the block)
                glm::ivec3 placePos = glm::ivec3(glm::floor(prevPos));
                if (world->getBlockGlobal(glm::vec3(placePos)) == 0) {
                    world->setBlockGlobal(glm::vec3(placePos), 1); // 1 = dirt
                    glm::ivec2 chunkPos = glm::ivec2(static_cast<int>(placePos.x / chunkSize), static_cast<int>(placePos.z / chunkSize));
                }
            } else {
                // Remove the block at the hit position
                world->setBlockGlobal(glm::vec3(blockPos), 0); // 0 = air
                glm::ivec2 chunkPos = glm::ivec2(static_cast<int>(blockPos.x / chunkSize), static_cast<int>(blockPos.z / chunkSize));
            }
            break;
        }
        prevPos = pos;
        pos += direction * step;
    }
    
}


