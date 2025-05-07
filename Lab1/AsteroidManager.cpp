#include "AsteroidManager.h"
#include "MainGame.h"
#include <glm/gtc/type_ptr.hpp>  

AsteroidManager& AsteroidManager::getInstance() {
	static AsteroidManager instance;
	return instance;
}

AsteroidManager::AsteroidManager() 
	: randomPosMin(-10.0f), randomPosMax(10.0f), // Default random positions
	randomScaleMin(1.0f), randomScaleMax(3.0f), // Default random scales
	randomForwardMin(5.0f), randomForwardMax(10.0f) // Default random forward directions
{
	srand(time(0));
}

glm::vec3 AsteroidManager::randomiseAsteroidPos() {
	float randX = randomPosMax + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (randomPosMin - (randomPosMax)))); // Randomise X position
	// Move the X of the asteroid outside the screen
	if (randX < 0)
		randX -= 35.0f;
	else
		randX += 35.0f;
	float randY = randomPosMax + static_cast <float>(rand()) / (static_cast <float>(RAND_MAX / (randomPosMin - (randomPosMax)))); // Randomise Y position
	// Move the Y of the asteroid outside the screen
	if (randY < 0)
		randY -= 10.0f;
	else
		randY += 10.0f;
	return glm::vec3(randX, randY, 0.0f);
}

glm::vec3 AsteroidManager::randomiseAsteroidScale() {
	float randS = randomScaleMax + static_cast <float>(rand()) / (static_cast <float>(RAND_MAX / (randomScaleMin - (randomScaleMax)))); // Randomise scale
	return glm::vec3(randS, randS, randS);
}

glm::vec3 AsteroidManager::randomiseAsteroidForwardDirection(glm::vec3 asteroidPos) {
	float randX = 0.0f;
	float randY = 0.0f;
	// Randomise forward direction based on the position of the asteroid
	// This is to make sure the asteroid moves towards somewhere onscreen instead of going further offscreen
	if (asteroidPos.x > 0 && asteroidPos.y > 0)
	{
		randX = -randomForwardMax + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (randomForwardMin - (-randomForwardMax))));
		randY = -randomForwardMax + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (randomForwardMin - (-randomForwardMax))));
	}
	else if (asteroidPos.x < 0 && asteroidPos.y > 0)
	{
		randX = randomForwardMax + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (randomForwardMin - (randomForwardMax))));
		randY = -randomForwardMax + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (randomForwardMin - (-randomForwardMax))));
	}
	else if (asteroidPos.x < 0 && asteroidPos.y < 0) 
	{
		randX = randomForwardMax + static_cast <float>(rand()) / (static_cast <float>(RAND_MAX / (randomForwardMin - (randomForwardMax))));
		randY = randomForwardMax + static_cast <float>(rand()) / (static_cast <float>(RAND_MAX / (randomForwardMin - (randomForwardMax))));
	}
	else {
		randX = -randomForwardMax + static_cast <float>(rand()) / (static_cast <float>(RAND_MAX / (randomForwardMin - (-randomForwardMax))));
		randY = randomForwardMax + static_cast <float>(rand()) / (static_cast <float>(RAND_MAX / (randomForwardMin - (randomForwardMax))));
	}
	return glm::vec3(randX, randY, 0.0f);
}

