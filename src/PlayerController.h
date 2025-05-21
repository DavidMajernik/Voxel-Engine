#pragma once

#include <glm/glm.hpp>
#include "World/World.h"
#include "World/World_Constants.h"

class PlayerController
{

public:

	PlayerController();

	~PlayerController();

	void RayCast(glm::vec3 camPos, glm::vec3 direction, std::unique_ptr<World>& world, bool place);

private: 

};