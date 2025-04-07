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
	float randX = -10.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0f - (-10.0f))));
	//if (randX < 0) {
	//	//randX -= 50.0f;
	//}
	//else {
	//	//randX += 50.0f;
	//}
	float randY = -10.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0f - (-10.0f))));
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
	float randX = -2.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0f - (-2.0f))));
	float randY = -2.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0f - (-2.0f))));
	return glm::vec3(randX, randY, 0.0f);
}

