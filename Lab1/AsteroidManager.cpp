#include "AsteroidManager.h"
#include "MainGame.h"
#include <glm/gtc/type_ptr.hpp>  

AsteroidManager& AsteroidManager::getInstance() {
	static AsteroidManager instance;
	return instance;
}

AsteroidManager::AsteroidManager() {
	srand(time(0));
}

// add code for randomising asteroids after they've spawned
glm::vec3 AsteroidManager::randomiseAsteroidPos() {
	float randX = -10.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0f - (-10.0f))));
	if (randX < 0) {
		randX -= 35.5f;
	}
	else {
		randX += 35.5f;
	}
	float randY = -10.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0f - (-10.0f))));
	if (randY < 0) {
		randY -= 10.0f;
	}
	else {
		randY += 10.0f;
	}
	return glm::vec3(randX, randY, 0.0f);
}

glm::vec3 AsteroidManager::randomiseAsteroidScale() {
	//srand(time(0));
	float randS = 3.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - (3.0f))));
	return glm::vec3(randS, randS, randS);
}

glm::vec3 AsteroidManager::randomiseAsteroidForwardDirection(glm::vec3 asteroidPos) {
	//srand(time(0));
	float randX = 0.0f;
	float randY = 0.0f;
	if (asteroidPos.x > 0 && asteroidPos.y > 0)
	{
		randX = -10.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0f - (-10.0f))));
		randY = -10.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0f - (-10.0f))));
	}
	else if (asteroidPos.x < 0 && asteroidPos.y > 0)
	{
		randX = 10.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0f - (10.0f))));
		randY = -10.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0f - (-10.0f))));
	}
	else if (asteroidPos.x < 0 && asteroidPos.y < 0) 
	{
		randX = 10.0f + static_cast <float>(rand()) / (static_cast <float>(RAND_MAX / (5.0f - (10.0f))));
		randY = 10.0f + static_cast <float>(rand()) / (static_cast <float>(RAND_MAX / (5.0f - (10.0f))));
	}
	else {
		randX = -10.0f + static_cast <float>(rand()) / (static_cast <float>(RAND_MAX / (5.0f - (-10.0f))));
		randY = 10.0f + static_cast <float>(rand()) / (static_cast <float>(RAND_MAX / (5.0f - (10.0f))));
	}
	return glm::vec3(randX, randY, 0.0f);
}

