#include "AsteroidManager.h"
#include "MainGame.h"
#include <glm/gtc/type_ptr.hpp>  

AsteroidManager& AsteroidManager::getInstance() {
	static AsteroidManager instance;
	return instance;
}

// add code for randomising asteroids after they've spawned
glm::vec3 AsteroidManager::randomiseAsteroidPos() {
	srand(time(0));
	float randX = -10 + rand() % (10 - (-10) + 1);
	//if (randX < 0) {
	//	//randX -= 50.0f;
	//}
	//else {
	//	//randX += 50.0f;
	//}
	float randY = -10 + rand() % (10 - (-10) + 1);
	//if (randY < 0) {
	//	//randY -= 35.0f;
	//}
	//else {
	//	//randY += 35.0f;
	//}
	return glm::vec3(randX, randY, 0.0f);
}

glm::vec3 AsteroidManager::randomiseAsteroidForwardDirection() {
	srand(time(0));
	float randX = -1 + rand() % (1 - (-1) + 1);
	float randY = -1 + rand() % (1 - (-1) + 1);
	return glm::vec3(randX, randY, 0.0f);
}

