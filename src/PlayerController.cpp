#include "PlayerController.h"

PlayerController::PlayerController() {}

PlayerController::~PlayerController() {}

void PlayerController::RayCast(glm::vec3 camPos, glm::vec3 direction, std::unique_ptr<World>& world, bool place, glm::vec3& hitBlockPos, bool buttonPress) {
    const float maxDistance = 100.0f;
    const float interactDistance = 100.0f;
    direction = glm::normalize(direction);

    glm::ivec3 blockPos = glm::ivec3(glm::floor(camPos));
	blockPos = glm::ivec3(blockPos.x, blockPos.y, blockPos.z);
    glm::ivec3 prevBlockPos = blockPos;
    glm::vec3 rayPos = camPos;

    // Calculate step and initial tMax/tDelta for each axis
    glm::ivec3 step;
    glm::vec3 tMax;
    glm::vec3 tDelta;

    for (int i = 0; i < 3; ++i) {
        if (direction[i] > 0) {
            step[i] = 1;
            tMax[i] = ((blockPos[i] + 1.0f) - rayPos[i]) / direction[i];
            tDelta[i] = 1.0f / direction[i];
        }
        else if (direction[i] < 0) {
            step[i] = -1;
            tMax[i] = (rayPos[i] - blockPos[i]) / -direction[i];
            tDelta[i] = 1.0f / -direction[i];
        }
        else {
            step[i] = 0;
            tMax[i] = std::numeric_limits<float>::max();
            tDelta[i] = std::numeric_limits<float>::max();
        }
    }

    float traveled = 0.0f;

    while (traveled < maxDistance) {
        uint8_t block = world->getBlockGlobal(glm::vec3(blockPos));
        if (block != 0) {
            hitBlockPos = glm::vec3(blockPos.x + 1, blockPos.y, blockPos.z + 1);
            if (buttonPress && traveled <= interactDistance) {
                if (place) {
                    // Place block at previous position (the last empty block)
                    if (world->getBlockGlobal(glm::vec3(prevBlockPos)) == 0) {
                        world->setBlockGlobal(glm::vec3(prevBlockPos), 1);
                    }
                }
                else {
                    world->setBlockGlobal(glm::vec3(blockPos), 0);
                }
            }
            break;
        }

        prevBlockPos = blockPos;

        // Advance to next voxel boundary
        if (tMax.x < tMax.y) {
            if (tMax.x < tMax.z) {
                blockPos.x += step.x;
                traveled = tMax.x;
                tMax.x += tDelta.x;
            }
            else {
                blockPos.z += step.z;
                traveled = tMax.z;
                tMax.z += tDelta.z;
            }
        }
        else {
            if (tMax.y < tMax.z) {
                blockPos.y += step.y;
                traveled = tMax.y;
                tMax.y += tDelta.y;
            }
            else {
                blockPos.z += step.z;
                traveled = tMax.z;
                tMax.z += tDelta.z;
            }
        }
    }
}


