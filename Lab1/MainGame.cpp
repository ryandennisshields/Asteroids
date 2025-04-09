#include "MainGame.h"
#include "Camera.h"
#include "DLLManager.h"
#include <glm/gtc/type_ptr.hpp>  
#include <iostream>
#include <string>


MainGame::MainGame()
	: _gameDisplay("OpenGL Game", 1980, 1024), // Initialize the display wrapper
	_gameState(GameState::PLAY), counter(0.0f) 
{
	fixedTimeStep = 1.0f / getRefreshRate(); // Dynamically set refresh-based time step
}

MainGame::~MainGame()
{
}

void MainGame::run()
{
	initSystems(); 
	gameLoop();
}

void MainGame::initSystems() {
	loadMeshes();
	loadTextures();
	setupUBOs();
	loadShaders();
	setupCamera();
	//setupTransforms();
	//createShip();
	//createAsteroid();
	loadPhysicsEngine();
	initShip();
	//initAsteroid(AsteroidManager::getInstance().randomiseAsteroidPos());
}

void MainGame::initShip() {
	shipTransform = Transform(glm::vec3(0.0f, 0.0f, 0.0f));
	ship = new GameObject(&shipMesh, &shipTransform, ShaderManager::getInstance().getShader("ADS").get());
}

void MainGame::initAsteroid(glm::vec3 spawnPosition, glm::vec3 spawnScale) {
	Transform* asteroidTransform = new Transform(spawnPosition, glm::vec3(0.0f, 0.0f, 0.0f), spawnScale); // Spawn in randomised position
	GameObject* asteroid = new GameObject(&asteroidMesh, asteroidTransform, ShaderManager::getInstance().getShader("ADS").get());
	asteroid->forwardDirection = glm::vec3(AsteroidManager::getInstance().randomiseAsteroidForwardDirection());
	asteroids.emplace_back(*asteroid);
	std::cout << "Asteroid created at: " << asteroid->transform->GetPos().x << ", " << asteroid->transform->GetPos().y << ", " << asteroid->transform->GetPos().z << std::endl;
	for (auto& asteroid : asteroids) {
		std::cout << "Asteroids: " << asteroid.transform->GetPos().x << ", " << asteroid.transform->GetPos().y << ", " << asteroid.transform->GetPos().z << std::endl;
	}
}

float MainGame::getRefreshRate() {
	SDL_DisplayMode mode;
	if (SDL_GetCurrentDisplayMode(0, &mode) == 0 && mode.refresh_rate > 0) {
		float rate = static_cast<float>(mode.refresh_rate);
		std::cout << "[DEBUG] Detected Refresh Rate: " << rate << " Hz" << std::endl;
		return rate;
	}
	std::cout << "[DEBUG] Could not detect refresh rate. Defaulting to 60 Hz." << std::endl;
	return 60.0f; // Fallback if query fails or returns 0
}

void MainGame::calculateDeltaTime() {
	float currentFrameTime = SDL_GetTicks() / 1000.0f; // Get current time in seconds
	deltaTime = currentFrameTime - lastFrameTime;
	lastFrameTime = currentFrameTime;
}

void MainGame::loadPhysicsEngine() {
	if (!DLLManager::getInstance().loadDLL("PhysicsEngine.dll")) {
		std::cerr << "Failed to load PhysicsEngine.dll" << std::endl;
		return;
	}

	// Retrieve and call HelloWorld function from DLL (test linkage)
	//typedef void (*HelloWorldFunc)();
	//HelloWorldFunc HelloWorld = DLLManager::getInstance().getFunction<HelloWorldFunc>("PhysicsEngine.dll", "HelloWorld");
	HelloWorld = DLLManager::getInstance().getFunction<void(*)()>("PhysicsEngine.dll", "HelloWorld");

	if (HelloWorld) {
		HelloWorld(); // Call function from DLL
	}
	else {
		std::cerr << "Failed to retrieve HelloWorld function!" << std::endl;
	}

	// Retrieve physics functions using the template method
	setForwardDirection = DLLManager::getInstance().getFunction<void(*)(GameObject*, glm::vec3)>("PhysicsEngine.dll", "setForwardDirection");
	applyThrust = DLLManager::getInstance().getFunction<void(*)(GameObject*, float)>("PhysicsEngine.dll", "applyThrust");
	updatePhysics = DLLManager::getInstance().getFunction<void(*)(GameObject*, float)>("PhysicsEngine.dll", "updatePhysics");

	if (!setForwardDirection || !applyThrust || !updatePhysics) {
		std::cerr << "Failed to retrieve physics functions!" << std::endl;
	}

	checkCollisionRadius = DLLManager::getInstance().getFunction<bool(*)(const GameObject*, const GameObject*, float, float)>("PhysicsEngine.dll", "checkCollisionRadius");
	checkCollisionAABB = DLLManager::getInstance().getFunction<bool(*)(const GameObject*, const GameObject*, const glm::vec3&, const glm::vec3&)>("PhysicsEngine.dll", "checkCollisionAABB");

	if (!checkCollisionRadius || !checkCollisionAABB) {
		std::cerr << "Failed to retrieve collision functions!" << std::endl;
	}
}


void MainGame::loadPhysicsEngineUnsafe() {
	DLLManager::getInstance().loadDLL("PhysicsEngine.dll"); // No check if successful

	using HelloWorldFunc = void(*)(); // functions stores, even if only local. 
	HelloWorldFunc HelloWorld = DLLManager::getInstance().getFunction<HelloWorldFunc>("PhysicsEngine.dll", "HelloWorld");

	(*HelloWorld)();  // Explicit dereferencing
	HelloWorld();  // Calls function directly with impicit derefernecing (no nullptr check)
}

// 🔹 Loads Meshes
void MainGame::loadMeshes() {
	shipMesh.loadModel("..\\res\\ship.obj");
	asteroidMesh.loadModel("..\\res\\asteroid.obj");
	laserMesh.loadModel("..\\res\\laser.obj");
}

// 🔹 Loads Textures
void MainGame::loadTextures() {
	TextManager::getInstance().init();
	texture.init("..\\res\\metal.jpg");
}

// 🔹 Sets up UBOs for matrix data
void MainGame::setupUBOs() {
	UBOManager::getInstance().createUBO("Matrices", sizeof(glm::mat4) * 3, 0);

	// Initialize with identity matrices to avoid garbage data
	glm::mat4 identity = glm::mat4(1.0f);
	UBOManager::getInstance().updateUBOData("Matrices", 0, glm::value_ptr(identity), sizeof(glm::mat4));
	UBOManager::getInstance().updateUBOData("Matrices", sizeof(glm::mat4), glm::value_ptr(identity), sizeof(glm::mat4));
	UBOManager::getInstance().updateUBOData("Matrices", sizeof(glm::mat4) * 2, glm::value_ptr(identity), sizeof(glm::mat4));
}

// 🔹 Loads and sets up shaders
void MainGame::loadShaders() {
	ShaderManager::getInstance().loadShader("glyphs", "..\\res\\glyphs.vert", "..\\res\\glyphs.frag");
	ShaderManager::getInstance().loadShader("ADS", "..\\res\\ADS.vert", "..\\res\\ADS.frag");
	setActiveShader("ADS");

	// Bind UBO to Shader
	UBOManager::getInstance().bindUBOToShader("Matrices", ShaderManager::getInstance().getShader("ADS")->ID(), "Matrices");
}

// 🔹 Sets up Camera
void MainGame::setupCamera() {
	myCamera.initCamera(glm::vec3(0, 0, -75), 100.0f,
		(float)_gameDisplay.getWidth() / _gameDisplay.getHeight(), 0.01f, 1000.0f);
}

// 🔹 Sets up Transforms for objects
//void MainGame::setupTransforms() {
//	TransformManager::getInstance().addTransform("asteroid", Transform(glm::vec3(0, 50, 0),
//		glm::vec3(0, 0, 0),
//		glm::vec3(1, 1, 1)));
//	TransformManager::getInstance().addTransform("ship", Transform(glm::vec3(0, 0, 0),
//		glm::vec3(0, 0, 0),
//		glm::vec3(1, 1, 1)));
//}

//void MainGame::createShip() { //this can be improved, we might return to this at some point and make it implicit
//
//	Transform* transform = &TransformManager::getInstance().getTransform("ship");
//	Shader* shader = ShaderManager::getInstance().getShader("ADS").get();
//
//	// make sure transform exists
//	// assert will terminate the program (usually with a message quoting the assert statement) 
//	// if its argument turns out to be false.
//	assert(transform && "TransformManager failed to return a transform!");
//	assert(shader && "ShaderManager failed to return a shader!");
//
//	gameObjects.emplace_back(&ship, transform, shader);
//}

//void MainGame::createAsteroid() {
//
//	Transform* transform = &TransformManager::getInstance().getTransform("asteroid");
//	Shader* shader = ShaderManager::getInstance().getShader("ADS").get();
//
//	// make sure transform exists
//	// assert will terminate the program (usually with a message quoting the assert statement) 
//	// if its argument turns out to be false.
//	assert(transform && "TransformManager failed to return a transform!");
//	assert(shader && "ShaderManager failed to return a shader!");
//
//	gameObjects.emplace_back(&asteroid, transform, shader);
//
//	AsteroidManager::getInstance().asteroids.emplace_back(&asteroid, transform, shader);
//}

void MainGame::gameLoop() {
	while (_gameState != GameState::EXIT) {
		calculateDeltaTime();
		processInput();
		updatePlayer(deltaTime);
		updateMovement(deltaTime);
		drawGame();
	}
}


void MainGame::processInput()
{
	SDL_Event evnt;

	while (SDL_PollEvent(&evnt)) // Get and process events
	{
		switch (evnt.type)
		{
		case SDL_QUIT:
			_gameState = GameState::EXIT;
			break;

		case SDL_KEYDOWN:
			handleKeyPress(evnt.key.keysym.sym);
			break;
		}
	}
}

void MainGame::handleKeyPress(SDL_Keycode key) {
	if (!ship) return;  // Ensure player exists before applying physics

	Transform& shipTransform = *(ship->transform); // Access player's transform

	std::cout << "Key Pressed: " << key << std::endl; // DEBUG

	switch (key) {
	case SDLK_SPACE:
		if (fireDelay <= 0.0f) {
			fireLaser();
			fireDelay = 1.0f;
		}
		break;
	case SDLK_w:  // Move forward (apply thrust)
		if (applyThrust) {
			applyThrust(ship, 1.0f);  // Apply forward force
		}
		break;
	case SDLK_s:  // Move backward (apply negative thrust)
		if (applyThrust) {
			applyThrust(ship, -1.0f);
		}
		break;

	case SDLK_a:  // Rotate left (increase Y rotation)
		if (setForwardDirection) {
			//glm::vec3 newRotation = shipTransform.GetRot()->y + glm::vec3(0.0f, glm::radians(5.0f), 0.0f);
			//shipTransform.SetRot(newRotation);
			shipTransform.rot += glm::vec3(0.0f, 0.0f, glm::radians(-5.0f));
		}
		break;

	case SDLK_d:  // Rotate right (decrease Y rotation)
		if (setForwardDirection) {
			//glm::vec3 newRotation = shipTransform.GetRot()->y + glm::vec3(0.0f, glm::radians(-5.0f), 0.0f);
			//shipTransform.SetRot(newRotation);
			shipTransform.rot += glm::vec3(0.0f, 0.0f, glm::radians(5.0f));
		}
		break;
	}
	if (ship->velocity.x > 6.0f) {
		ship->velocity.x = 6.0f; // Cap velocity to 3.0f
	}
	if (ship->velocity.y > 6.0f) {
		ship->velocity.y = 6.0f; // Cap velocity to 3.0f
	}
	if (ship->velocity.x < -6.0f) {
		ship->velocity.x = -6.0f; // Cap velocity to -3.0f
	}
	if (ship->velocity.y < -6.0f) {
		ship->velocity.y = -6.0f; // Cap velocity to -3.0f
	}

	glm::vec3 rotation = ship->transform->rot;
	float yaw = rotation.z; // Assuming rotation around the Z-axis for 2D plane

	// Calculate the new forward direction based on yaw, ignoring the Z-axis
	glm::vec3 newForwardDirection;
	newForwardDirection.x = -sin(yaw);
	newForwardDirection.y = cos(yaw);
	newForwardDirection.z = 0.0f; // Ignore Z-axis

	// Normalize the direction vector
	newForwardDirection = glm::normalize(newForwardDirection);

	// Update the forward direction of the GameObject
	//ship->forwardDirection = newForwardDirection;
	setForwardDirection(ship, newForwardDirection); // Update forward direction

}

void MainGame::fireLaser() {
	Transform* laserTransform = new Transform(ship->transform->pos, ship->transform->rot); // Spawn in randomised position
	GameObject* laser = new GameObject(&laserMesh, laserTransform , ShaderManager::getInstance().getShader("ADS").get());
	laser->forwardDirection = glm::vec3(ship->forwardDirection);
	lasers.emplace_back(*laser);
}

void MainGame::updatePlayer(float deltaTime) {
	if (!ship) return;

	// Apply velocity to position
	shipTransform.pos += ship->velocity * deltaTime;

	// Debugging: Ensure position updates
	/*std::cout << "Player Position: "
		<< player->transform->GetPos()->x << '\n';*/

	glm::vec3 shipPos = ship->transform->GetPos();
	if (shipPos.x > 32.5f) {
		ship->transform->pos.x = -32.5f; // Wrap around
	}
	if (shipPos.x < -32.5f) {
		ship->transform->pos.x = 32.5f; // Wrap around
	}
	if (shipPos.y > 20.0f) {
		ship->transform->pos.y = -20.0f; // Wrap around
	}
	if (shipPos.y < -20.0f) {
		ship->transform->pos.y = 20.0f; // Wrap around
	}
	//std::cout << "ship position: " << ship->transform->GetPos().x << ", " << ship->transform->GetPos().y << ", " << ship->transform->GetPos().z << std::endl;
}

void MainGame::updateMovement(float deltaTime) {
	counter += deltaTime;
	if (counter > 5.0f) {
		initAsteroid(AsteroidManager::getInstance().randomiseAsteroidPos());
		counter = 0.0f;
	}

	if (fireDelay > 0.0f) {
		fireDelay -= deltaTime;
	}

	for (auto& obj : asteroids) {
		for (auto& laser : lasers) {
			if (checkCollisionAABB(&obj, &laser, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 (1.0f, 1.0f, 1.0f))) {
				// Handle collision
				std::cout << "Collision detected!" << std::endl;
				// Remove the laser and asteroid from their respective vectors
				Transform* asteroid = obj.transform;
				lasers.erase(lasers.begin() + (&laser - &lasers[0]));
				asteroids.erase(asteroids.begin() + (&obj - &asteroids[0]));
				if (asteroid->scale.z >= 1.0f) {
					initAsteroid(asteroid->pos, glm::vec3(asteroid->scale.x / 2, asteroid->scale.y / 2, asteroid->scale.z / 2));
					initAsteroid(asteroid->pos, glm::vec3(asteroid->scale.x / 2, asteroid->scale.y / 2, asteroid->scale.z / 2));
				}
			}
		}
	}
	//Transform& transform = TransformManager::getInstance().getTransform("susanna2");// make a mesh current e.g. mesh2
	//transform.SetPos(glm::vec3(5.0f - counter, 0.0f, 0.0f));// update current mesh
	//bool collision = checkCollisionAABB(&gameObjects[0], &gameObjects[1], glm::vec3(1.0f), glm::vec3(1.0f));
	//if (collision) {
	//	std::cout << "Collision detected!" << std::endl;
	//}
}


void MainGame::setActiveShader(const std::string& shaderTag) {
	if (ShaderManager::getInstance().getShader(shaderTag)) {
		activeShaderTag = shaderTag;
		std::cout << "Shader switched to: " << shaderTag << std::endl;
	}
	else {
		std::cerr << "Error: Shader not found - " << shaderTag << std::endl;
	}
}

void MainGame::renderGameObjects() { // this can be quickly improved for coursework (for our asteroids)
	Shader* currentShader = nullptr; // Track active shader to avoid redundant binds

	for (auto& obj : asteroids) {
		if (obj.shader != currentShader) {
			currentShader = obj.shader;
			currentShader->Bind(); // Bind shader only if switching
		}

		// Update UBO for this object's transform
		glm::mat4 model = obj.transform->GetModel();
		glm::mat4 view = myCamera.getView();
		glm::mat4 projection = myCamera.getProjection();

		UBOManager::getInstance().updateUBOData("Matrices", 0, glm::value_ptr(model), sizeof(glm::mat4));
		UBOManager::getInstance().updateUBOData("Matrices", sizeof(glm::mat4), glm::value_ptr(view), sizeof(glm::mat4));
		UBOManager::getInstance().updateUBOData("Matrices", sizeof(glm::mat4) * 2, glm::value_ptr(projection), sizeof(glm::mat4));

		obj.mesh->draw();

		glm::vec3 objPos = obj.transform->GetPos();
		if (objPos.x > 32.5f) {
			asteroids.erase(asteroids.begin() + (&obj - &asteroids[0])); // Remove object
		}
		if (objPos.x < -32.5f) {
			asteroids.erase(asteroids.begin() + (&obj - &asteroids[0])); // Remove object
		}
		if (objPos.y > 20.0f) {
			asteroids.erase(asteroids.begin() + (&obj - &asteroids[0])); // Remove object
		}
		if (objPos.y < -20.0f) {
			asteroids.erase(asteroids.begin() + (&obj - &asteroids[0])); // Remove object
		}

		obj.transform->pos += obj.forwardDirection * 0.5f * deltaTime; // Move asteroid forward
		//float angle = SDL_GetTicks() * 0.0001f; // Convert milliseconds to seconds
		//glUniform1f(glGetUniformLocation(currentShader->ID(), "angle"), angle);
	}

	for (auto& obj : lasers) {
		if (obj.shader != currentShader) {
			currentShader = obj.shader;
			currentShader->Bind(); // Bind shader only if switching
		}

		// Update UBO for this object's transform
		glm::mat4 model = obj.transform->GetModel();
		glm::mat4 view = myCamera.getView();
		glm::mat4 projection = myCamera.getProjection();

		UBOManager::getInstance().updateUBOData("Matrices", 0, glm::value_ptr(model), sizeof(glm::mat4));
		UBOManager::getInstance().updateUBOData("Matrices", sizeof(glm::mat4), glm::value_ptr(view), sizeof(glm::mat4));
		UBOManager::getInstance().updateUBOData("Matrices", sizeof(glm::mat4) * 2, glm::value_ptr(projection), sizeof(glm::mat4));

		obj.mesh->draw();

		obj.transform->pos += obj.forwardDirection * 6.0f * deltaTime; // Move asteroid forward
	}
}

void MainGame::renderPlayer() {
	if (!ship) {
		std::cerr << "Warning: Player pointer is null! Skipping render." << std::endl;
		return;
	}

	// Use the shader that was assigned to the player
	Shader* playerShader = ship->shader;
	if (!playerShader) {
		std::cerr << "Error: Player has no assigned shader!" << std::endl;
		return;
	}

	playerShader->Bind();

	glm::mat4 playerModel = ship->transform->GetModel();
	glm::mat4 view = myCamera.getView();
	glm::mat4 projection = myCamera.getProjection();

	UBOManager::getInstance().updateUBOData("Matrices", 0, glm::value_ptr(playerModel), sizeof(glm::mat4));
	UBOManager::getInstance().updateUBOData("Matrices", sizeof(glm::mat4), glm::value_ptr(view), sizeof(glm::mat4));
	UBOManager::getInstance().updateUBOData("Matrices", sizeof(glm::mat4) * 2, glm::value_ptr(projection), sizeof(glm::mat4));

	ship->mesh->draw();
}

void MainGame::clearScreenBuffer()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set clear color to black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear colour and depth buffer - set colour to colour defined in glClearColor
}

void MainGame::drawGame() {
	clearScreenBuffer();
	TextManager::getInstance().renderText(*ShaderManager::getInstance().getShader("glyphs").get(), "Hello World", 100.0f, 100.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	renderPlayer();
	renderGameObjects(); // Now handles full rendering
	_gameDisplay.swapBuffers();
}

