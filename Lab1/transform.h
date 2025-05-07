#pragma once

#include <glm/glm.hpp>

struct Transform 
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	glm::vec3 forwardDirection = glm::vec3(0, 1, 0); // Default forward direction
	glm::vec3 velocity = glm::vec3(0);
};