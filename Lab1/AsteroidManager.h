#pragma once

#include <SDL\SDL.h>
#include "ShaderManager.h"
#include "UBOManager.h"

class AsteroidManager
{
public:
	static AsteroidManager& getInstance();

	glm::vec3 randomiseAsteroidPos(); // Randomise position between many ranges outside the visible screen
	glm::vec3 randomiseAsteroidForwardDirection(glm::vec3 asteroidPos); // Randomise the forward direction
	glm::vec3 randomiseAsteroidScale(); // Randomise scale

private:
	AsteroidManager();
	~AsteroidManager() = default;

	float randomPosMin;
	float randomPosMax;
	float randomScaleMin;
	float randomScaleMax;
	float randomForwardMin;
	float randomForwardMax;
};
