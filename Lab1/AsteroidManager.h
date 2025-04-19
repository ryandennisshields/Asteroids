#pragma once

#include <SDL\SDL.h>
#include "MainGame.h"
#include "GameObject.h"
#include "TransformManager.h"
#include "ShaderManager.h"
#include "UBOManager.h"

class AsteroidManager
{
public:
	static AsteroidManager& getInstance();

	glm::vec3 randomiseAsteroidPos();
	glm::vec3 randomiseAsteroidForwardDirection(glm::vec3 asteroidPos);
	glm::vec3 randomiseAsteroidScale();

	//std::vector<GameObject> asteroids;
private:
	AsteroidManager() = default;
	~AsteroidManager() = default;
};
