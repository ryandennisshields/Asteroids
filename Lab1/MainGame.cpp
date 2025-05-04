#include "MainGame.h"
#include "Camera.h"
#include "DLLManager.h"
#include <glm/gtc/type_ptr.hpp>  
#include <iostream>
#include <string>


MainGame::MainGame()
	: _gameDisplay("OpenGL Game", 1980, 1024), // Initialize the display wrapper
	_gameState(GameState::PLAY), 
	spawnDelay(0.0f), fireDelay(0.0f), score(0)
{
	fixedTimeStep = 1.0f / getRefreshRate(); // Dynamically set refresh-based time step
}

MainGame::~MainGame()
{
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

void MainGame::run()
{
	initSystems(); 
	gameLoop();
}

void MainGame::initSystems() {
	initECS();
	loadMeshes();
	loadTextures();
	setupUBOs();
	loadShaders();
	setupCamera();
	loadPhysicsEngine();
	initShip();
	initAsteroid();
}

void MainGame::initECS() {
	coordinator.init();

	// Register components
	coordinator.registerComponent<Transform>();
	coordinator.registerComponent<Mesh>();

	// Register systems
	transformSystem = *coordinator.registerSystem<TransformSystem>();
	meshSystem = *coordinator.registerSystem<MeshSystem>();

	// Set system signatures
	Signature transformSignature;
	transformSignature.set(coordinator.getComponentType<Transform>());
	coordinator.setSystemSignature<TransformSystem>(transformSignature);

	Signature meshSignature;
	//meshSignature.set(coordinator.getComponentType<Transform>()); // Meshes need transforms for rendering
	meshSignature.set(coordinator.getComponentType<Mesh>());
	coordinator.setSystemSignature<MeshSystem>(meshSignature);

	//std::vector<Entity> entities(1);

	//for (auto& entity : entities) {
	//	entity = coordinator.createEntity(); // Create an entity
	//	coordinator.addComponent(
	//		entity,
	//		ECSTransform{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) });
	//}

	// create a variable in the .h file for entities
	// for creating the ship, asteroids, and lasers, create an entity for them and add suitable components
	// then, in rendering loop, render the ships, asteroids and lasers with their transform now controlled by a system using the transform component
}

// 🔹 Loads Meshes
void MainGame::loadMeshes() {
	shipMesh = Mesh("..\\res\\ship.obj");
	meshSystem.loadModel(shipMesh); // Load the ship mesh
	asteroidMesh = Mesh("..\\res\\asteroid.obj");
	meshSystem.loadModel(asteroidMesh); // Load the ship mesh
	laserMesh = Mesh("..\\res\\laser.obj");
	meshSystem.loadModel(laserMesh); // Load the ship mesh
}

// 🔹 Loads Textures
void MainGame::loadTextures() {
	shipTexture.init("..\\res\\metal.jpg");
	asteroidTexture.init("..\\res\\asteroidRock.jpg");
	laserTexture.init("..\\res\\laser.jpg");
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
	ShaderManager::getInstance().loadShader("ADS", "..\\res\\ADS.vert", "..\\res\\ADS.frag");
	ShaderManager::getInstance().loadShader("glyphs", "..\\res\\glyphs.vert", "..\\res\\glyphs.frag");
	setActiveShader("ADS");

	// Bind UBO to Shader
	UBOManager::getInstance().bindUBOToShader("Matrices", ShaderManager::getInstance().getShader("ADS")->ID(), "Matrices");
}

// 🔹 Sets up Camera
void MainGame::setupCamera() {
	myCamera.initCamera(glm::vec3(0, 0, -75), 100.0f,
		(float)_gameDisplay.getWidth() / _gameDisplay.getHeight(), 0.01f, 1000.0f);
}

void MainGame::loadPhysicsEngine() {
	if (!DLLManager::getInstance().loadDLL("PhysicsEngine.dll")) {
		std::cerr << "Failed to load PhysicsEngine.dll" << std::endl;
		return;
	}

	// Retrieve physics functions using the template method
	setForwardDirection = DLLManager::getInstance().getFunction<void(*)(Transform*, glm::vec3)>("PhysicsEngine.dll", "setForwardDirection");
	applyThrust = DLLManager::getInstance().getFunction<void(*)(Transform*, float)>("PhysicsEngine.dll", "applyThrust");
	updatePhysics = DLLManager::getInstance().getFunction<void(*)(Transform*, float)>("PhysicsEngine.dll", "updatePhysics");

	if (!setForwardDirection || !applyThrust || !updatePhysics) {
		std::cerr << "Failed to retrieve physics functions!" << std::endl;
	}

	checkCollisionRadius = DLLManager::getInstance().getFunction<bool(*)(const Transform*, const Transform*, float, float)>("PhysicsEngine.dll", "checkCollisionRadius");
	checkCollisionAABB = DLLManager::getInstance().getFunction<bool(*)(const Transform*, const Transform*, const glm::vec3&, const glm::vec3&)>("PhysicsEngine.dll", "checkCollisionAABB");

	if (!checkCollisionRadius || !checkCollisionAABB) {
		std::cerr << "Failed to retrieve collision functions!" << std::endl;
	}
}

void MainGame::initShip() {
	// Create an entity for the ship
	Entity shipEntity = coordinator.createEntity();
	//shipEntity = coordinator.createEntity(); // Create an entity

	// Add the ECSTransform component to the ship entity
	coordinator.addComponent(shipEntity, Transform{
		glm::vec3(0.0f, 0.0f, 0.0f), // Position
		glm::vec3(0.0f, 0.0f, 0.0f), // Rotation
		glm::vec3(1.0f, 1.0f, 1.0f)  // Scale
		});
	coordinator.addComponent(shipEntity, shipMesh); // Add the mesh component to the ship entity

	shipTransform = &coordinator.getComponent<Transform>(shipEntity); // Get the transform component for the ship entity
	ship = &shipEntity; // Store the ship entity in the ship pointer
	//ship = new GameObject(&shipMesh, shipTransform, ShaderManager::getInstance().getShader("ADS").get());
}

void MainGame::initAsteroid() {
	Entity asteroidEntity = coordinator.createEntity();

	AsteroidManager& asteroidManager = AsteroidManager::getInstance();
	coordinator.addComponent(asteroidEntity, Transform{
		asteroidManager.randomiseAsteroidPos(),
		glm::vec3(0.0f, 0.0f, 0.0f),
		asteroidManager.randomiseAsteroidScale()
		});
	coordinator.addComponent(asteroidEntity, asteroidMesh); // Add the mesh component to the ship entity

	Transform* asteroidTransform = &coordinator.getComponent<Transform>(asteroidEntity); // Get the transform component for the asteroid entity
	//GameObject* asteroid = new GameObject(&asteroidMesh, asteroidTransform, ShaderManager::getInstance().getShader("ADS").get());
	asteroidTransform->forwardDirection = glm::vec3(asteroidManager.randomiseAsteroidForwardDirection(asteroidTransform->position));
	asteroids.emplace_back(asteroidEntity);

	gameEntities.push_back(asteroidEntity);
	//std::cout << "[DEBUG] Asteroid Created at Position: " << asteroidTransform->pos.x << ", " << asteroidTransform->pos.y << ", " << asteroidTransform->pos.z << std::endl;
}

void MainGame::gameLoop() {
	while (_gameState != GameState::EXIT) {
		processInput();
		if (_gameState != GameState::GAMEOVER) {
			calculateDeltaTime();
			updatePlayer(deltaTime);
			update(deltaTime);
		}
		drawGame();
	}
}

void MainGame::processInput()
{
	SDL_Event evnt;

	while (SDL_PollEvent(&evnt)) {
		if (evnt.type == SDL_QUIT) {
			_gameState = GameState::EXIT;
		}
	}

	const Uint8* keystates = SDL_GetKeyboardState(nullptr);

	handleKeyPress(keystates);
}


void MainGame::handleKeyPress(const Uint8* keystates) {
	if (!ship) return; // Ensure ship exists before applying physics

	if (keystates[SDL_SCANCODE_ESCAPE]) {
		_gameState = GameState::EXIT;
	}

	float thrust = 10.0f;
	float velocityCap = 6.0f;
	float rotationSpeed = 100.0f;
	float fireRate = 0.5f;

	if (keystates[SDL_SCANCODE_W]) {
		if (applyThrust) {
			applyThrust(shipTransform, thrust * deltaTime);
		}
	}

	if (keystates[SDL_SCANCODE_S]) {
		if (applyThrust) {
			applyThrust(shipTransform, -thrust * deltaTime);
		}
	}

	// Cap velocity
	shipTransform->velocity.x = std::clamp(shipTransform->velocity.x, -velocityCap, velocityCap);
	shipTransform->velocity.y = std::clamp(shipTransform->velocity.y, -velocityCap, velocityCap);

	if (keystates[SDL_SCANCODE_A]) {
		if (setForwardDirection) {
			shipTransform->rotation.z -= glm::radians(rotationSpeed * deltaTime);
			setForwardDirection(shipTransform, shipTransform->rotation);
		}
	}

	if (keystates[SDL_SCANCODE_D]) {
		if (setForwardDirection) {
			shipTransform->rotation.z += glm::radians(rotationSpeed * deltaTime);
			setForwardDirection(shipTransform, shipTransform->rotation);
		}
	}

	if (keystates[SDL_SCANCODE_SPACE]) {
		if (fireDelay <= 0.0f) {
			fireLaser();
			fireDelay = fireRate; // Reset fire delay
		}
	}
}


void MainGame::fireLaser() {
	Entity laserEntity = coordinator.createEntity();
	//shipEntity = coordinator.createEntity(); // Create an entity

	// Add the ECSTransform component to the ship entity
	coordinator.addComponent(laserEntity, Transform{
		shipTransform->position , // Position
		shipTransform->rotation, // Rotation
		glm::vec3(1.0f, 1.0f, 1.0f)  // Scale
		});
	coordinator.addComponent(laserEntity, laserMesh); // Add the mesh component to the ship entity

	Transform* laserTransform = &coordinator.getComponent<Transform>(laserEntity);
	//GameObject* laser = new GameObject(&laserMesh, laserTransform , ShaderManager::getInstance().getShader("ADS").get());
	laserTransform->forwardDirection = glm::vec3(shipTransform->forwardDirection);
	lasers.emplace_back(laserEntity);

	gameEntities.push_back(laserEntity);
}

void MainGame::calculateDeltaTime() {
	float currentFrameTime = SDL_GetTicks() / 1000.0f; // Get current time in seconds
	deltaTime = currentFrameTime - lastFrameTime;
	lastFrameTime = currentFrameTime;
}

void MainGame::updatePlayer(float deltaTime) {
	if (!ship) return;

	//auto& shipTransform = coordinator.getComponent<Transform>(gameEntities[0]);
	// Apply velocity to position
	shipTransform->position += shipTransform->velocity * deltaTime;

	// Debugging: Ensure position updates
	/*std::cout << "Player Position: "
		<< player->transform->GetPos()->x << '\n';*/

	float wrapPosX = 32.5f;
	float wrapPosY = 20.0f;

	glm::vec3 shipPos = shipTransform->position;
	if (shipPos.x > wrapPosX)
		shipTransform->position.x = -wrapPosX;
	if (shipPos.x < -wrapPosX) 
		shipTransform->position.x = wrapPosX;
	if (shipPos.y > wrapPosY)
		shipTransform->position.y = -wrapPosY;
	if (shipPos.y < -wrapPosY) 
		shipTransform->position.y = wrapPosY;
	//std::cout << "ship position: " << ship->transform->GetPos().x << ", " << ship->transform->GetPos().y << ", " << ship->transform->GetPos().z << std::endl;
}

void MainGame::update(float deltaTime) {
	if (spawnDelay > 0.0f)
		spawnDelay -= deltaTime;

	if (fireDelay > 0.0f) 
		fireDelay -= deltaTime;

	float spawnRate = 0.2f; // Spawn rate in seconds

	if (spawnDelay <= 0.0f) {
		initAsteroid();
		spawnDelay = spawnRate;
	}

	float deleteBoundsX = 50.0f;
	float deleteBoundsY = 40.0f;

	float asteroidSpeed = 0.5f;

	for (auto& asteroid : asteroids) {

		auto* asteroidTransform = &coordinator.getComponent<Transform>(asteroid);

		asteroidTransform->position += asteroidTransform->forwardDirection * asteroidSpeed * deltaTime; // Move asteroid forward

		if (asteroidTransform->position.x > deleteBoundsX) {
			coordinator.destroyEntity(asteroid);
			asteroids.erase(asteroids.begin() + (&asteroid - &asteroids[0])); // Remove asteroidect
		}
		if (asteroidTransform->position.x < -deleteBoundsX) {
			coordinator.destroyEntity(asteroid);
			asteroids.erase(asteroids.begin() + (&asteroid - &asteroids[0])); // Remove asteroidect
		}
		if (asteroidTransform->position.y > deleteBoundsY) {
			coordinator.destroyEntity(asteroid);
			asteroids.erase(asteroids.begin() + (&asteroid - &asteroids[0])); // Remove asteroidect
		}
		if (asteroidTransform->position.y < -deleteBoundsY) {
			coordinator.destroyEntity(asteroid);
			asteroids.erase(asteroids.begin() + (&asteroid - &asteroids[0])); // Remove asteroidect
		}

		for (auto& laser : lasers) {

			auto* laserTransform = &coordinator.getComponent<Transform>(laser);

			if (checkCollisionAABB(asteroidTransform, laserTransform, asteroidTransform->scale, glm::vec3 (1.0f, 1.0f, 1.0f))) {
				// Handle collision
				//std::cout << "Collision detected!" << std::endl;
				// Remove the laser and asteroid from their respective vectors
				//Transform* asteroid = asteroid.transform;
				//if (asteroid->scale.z >= 1.0f) {
					//initAsteroid(asteroid->pos, glm::vec3(asteroid->scale.x / 2, asteroid->scale.y / 2, asteroid->scale.z / 2));
				//}
				score += 100;
				asteroids.erase(asteroids.begin() + (&asteroid - &asteroids[0]));
				lasers.erase(lasers.begin() + (&laser - &lasers[0]));
			}
		}
		if (checkCollisionAABB(asteroidTransform, shipTransform, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)))
		{
			lasers.clear();
			asteroids.clear();
			_gameState = GameState::GAMEOVER;
		}
	}

	float laserSpeed = 15.0f;

	for (auto& laser : lasers) {

		auto* laserTransform = &coordinator.getComponent<Transform>(laser);

		laserTransform->position += laserTransform->forwardDirection * laserSpeed * deltaTime; // Move laser forward

		if (laserTransform->position.x > deleteBoundsX) {
			coordinator.destroyEntity(laser);
			lasers.erase(lasers.begin() + (&laser - &lasers[0])); // Remove object
		}
		if (laserTransform->position.x < -deleteBoundsX) {
			coordinator.destroyEntity(laser);
			lasers.erase(lasers.begin() + (&laser - &lasers[0])); // Remove object
		}
		if (laserTransform->position.y > deleteBoundsY) {
			coordinator.destroyEntity(laser);
			lasers.erase(lasers.begin() + (&laser - &lasers[0])); // Remove object
		}
		if (laserTransform->position.y < -deleteBoundsY) {
			coordinator.destroyEntity(laser);
			lasers.erase(lasers.begin() + (&laser - &lasers[0])); // Remove object
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

void MainGame::clearScreenBuffer()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set clear color to black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear colour and depth buffer - set colour to colour defined in glClearColor
}

void MainGame::renderGameObjects() {
	//
	// NOTE
	// I attempted to make this just a single for loop with gameEntities, but that would've required making both the Shader
	// and Texture part of the ECS, which was outside my scope for the amount of time I had remaining.
	//
	Entity ship = gameEntities[0]; // Get the ship entity

	Shader* currentShader = nullptr;

	auto& transform = coordinator.getComponent<Transform>(ship);
	auto& mesh = coordinator.getComponent<Mesh>(ship);

	currentShader = ShaderManager::getInstance().getShader("ADS").get();
	currentShader->Bind();

	shipTexture.Bind(0);

	glm::mat4 model = transformSystem.update(ship);
	glm::mat4 view = myCamera.getView();
	glm::mat4 projection = myCamera.getProjection();

	UBOManager::getInstance().updateUBOData("Matrices", 0, glm::value_ptr(model), sizeof(glm::mat4));
	UBOManager::getInstance().updateUBOData("Matrices", sizeof(glm::mat4), glm::value_ptr(view), sizeof(glm::mat4));
	UBOManager::getInstance().updateUBOData("Matrices", sizeof(glm::mat4) * 2, glm::value_ptr(projection), sizeof(glm::mat4));

	meshSystem.render(mesh);

	for (auto& asteroid : asteroids) {
		auto& transform = coordinator.getComponent<Transform>(asteroid);
		auto& mesh = coordinator.getComponent<Mesh>(asteroid);

		currentShader = ShaderManager::getInstance().getShader("ADS").get();
		currentShader->Bind();

		asteroidTexture.Bind(0);

		glm::mat4 model = transformSystem.update(asteroid);
		glm::mat4 view = myCamera.getView();
		glm::mat4 projection = myCamera.getProjection();

		UBOManager::getInstance().updateUBOData("Matrices", 0, glm::value_ptr(model), sizeof(glm::mat4));
		UBOManager::getInstance().updateUBOData("Matrices", sizeof(glm::mat4), glm::value_ptr(view), sizeof(glm::mat4));
		UBOManager::getInstance().updateUBOData("Matrices", sizeof(glm::mat4) * 2, glm::value_ptr(projection), sizeof(glm::mat4));

		meshSystem.render(mesh);
	}
	for (auto& laser : lasers) {
		auto& transform = coordinator.getComponent<Transform>(laser);
		auto& mesh = coordinator.getComponent<Mesh>(laser);

		currentShader = ShaderManager::getInstance().getShader("ADS").get();
		currentShader->Bind();

		laserTexture.Bind(0);

		glm::mat4 model = transformSystem.update(laser);
		glm::mat4 view = myCamera.getView();
		glm::mat4 projection = myCamera.getProjection();

		UBOManager::getInstance().updateUBOData("Matrices", 0, glm::value_ptr(model), sizeof(glm::mat4));
		UBOManager::getInstance().updateUBOData("Matrices", sizeof(glm::mat4), glm::value_ptr(view), sizeof(glm::mat4));
		UBOManager::getInstance().updateUBOData("Matrices", sizeof(glm::mat4) * 2, glm::value_ptr(projection), sizeof(glm::mat4));

		meshSystem.render(mesh);
	}
}


void MainGame::drawGame() {
	TextManager& textManager = TextManager::getInstance();
	clearScreenBuffer();
	if (_gameState == GameState::PLAY)
	{
		textManager.renderText(*ShaderManager::getInstance().getShader("glyphs").get(), std::to_string(score), 125.0f, 500.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
		renderGameObjects(); // Now handles full rendering
	}
	if (_gameState == GameState::GAMEOVER) 
	{
		textManager.renderText(*ShaderManager::getInstance().getShader("glyphs").get(), "Game Over", 275.0f, 400.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		textManager.renderText(*ShaderManager::getInstance().getShader("glyphs").get(), std::to_string(score), 275.0f, 300.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
		textManager.renderText(*ShaderManager::getInstance().getShader("glyphs").get(), "Press ESC to exit", 275.0f, 200.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
	}
	_gameDisplay.swapBuffers();
}

