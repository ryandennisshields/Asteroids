#include "AsteroidManager.h"
#include "MainGame.h"
#include <glm/gtc/type_ptr.hpp>  

AsteroidManager& AsteroidManager::getInstance() {
	static AsteroidManager instance;
	return instance;
}

AsteroidManager::AsteroidManager() 
	: randomPosMin(-10.0f), randomPosMax(10.0f),
	randomScaleMin(1.0f), randomScaleMax(3.0f),
	randomForwardMin(5.0f), randomForwardMax(10.0f)
{
	srand(time(0));
}

glm::vec3 AsteroidManager::randomiseAsteroidPos() {
	float randX = randomPosMax + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (randomPosMin - (randomPosMax))));
	if (randX < 0)
		randX -= 35.0f;
	else
		randX += 35.0f;
	float randY = randomPosMax + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (randomPosMin - (randomPosMax))));
	if (randY < 0)
		randY -= 10.0f;
	else
		randY += 10.0f;
	return glm::vec3(randX, randY, 0.0f);
}

glm::vec3 AsteroidManager::randomiseAsteroidScale() {
	float randS = randomScaleMax + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (randomScaleMin - (randomScaleMax))));
	return glm::vec3(randS, randS, randS);
}

glm::vec3 AsteroidManager::randomiseAsteroidForwardDirection(glm::vec3 asteroidPos) {
	float randX = 0.0f;
	float randY = 0.0f;
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

