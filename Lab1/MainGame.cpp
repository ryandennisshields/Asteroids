#include "MainGame.h"
#include "Camera.h"
#include "DLLManager.h"
#include <glm/gtc/type_ptr.hpp>  
#include <iostream>
#include <string>


MainGame::MainGame()
	: _gameDisplay("OpenGL Game", 1920, 1080), // Initialize the display wrapper
	_gameState(GameState::PLAY), 
	spawnDelay(0.0f), fireDelay(0.0f), score(0),
	asteroids(), lasers()
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
	initShip();
	initAsteroid();
	loadPhysicsEngine();
}

// Initializes the ECS (Entity-Component-System) framework
void MainGame::initECS() {

	coordinator.init();

	// Register components
	coordinator.registerComponent<Transform>();
	coordinator.registerComponent<Mesh>();
	coordinator.registerComponent<Texture>();

	// Register systems
	transformSystem = *coordinator.registerSystem<TransformSystem>();
	meshSystem = *coordinator.registerSystem<MeshSystem>();
	textureSystem = *coordinator.registerSystem<TextureSystem>();

	// Set system signatures
	Signature transformSignature;
	transformSignature.set(coordinator.getComponentType<Transform>());
	coordinator.setSystemSignature<TransformSystem>(transformSignature);

	Signature meshSignature;
	meshSignature.set(coordinator.getComponentType<Mesh>());
	coordinator.setSystemSignature<MeshSystem>(meshSignature);

	Signature textureSignature;
	textureSignature.set(coordinator.getComponentType<Texture>());
	coordinator.setSystemSignature<TextureSystem>(textureSignature);
}

// Loads Meshes
void MainGame::loadMeshes() {
	shipMesh = Mesh("..\\res\\ship.obj");
	meshSystem.loadModel(shipMesh); // Load the ship mesh

	asteroidMesh = Mesh("..\\res\\asteroid.obj");
	meshSystem.loadModel(asteroidMesh); // Load the asteroid mesh

	laserMesh = Mesh("..\\res\\laser.obj");
	meshSystem.loadModel(laserMesh); // Load the laser mesh
}

// Loads Textures
void MainGame::loadTextures() {
	shipTexture = Texture("..\\res\\metal.jpg");
	textureSystem.loadTexture(shipTexture); // Load the ship texture

	asteroidTexture = Texture("..\\res\\asteroidrock.jpg");
	textureSystem.loadTexture(asteroidTexture); // Load the asteroid texture

	laserTexture = Texture("..\\res\\laser.jpg");
	textureSystem.loadTexture(laserTexture); // Load the laser texture
}


// Sets up UBOs for matrix data
void MainGame::setupUBOs() {
	UBOManager::getInstance().createUBO("Matrices", sizeof(glm::mat4) * 3, 0);

	// Initialize with identity matrices to avoid garbage data
	glm::mat4 identity = glm::mat4(1.0f);
	UBOManager::getInstance().updateUBOData("Matrices", 0, glm::value_ptr(identity), sizeof(glm::mat4));
	UBOManager::getInstance().updateUBOData("Matrices", sizeof(glm::mat4), glm::value_ptr(identity), sizeof(glm::mat4));
	UBOManager::getInstance().updateUBOData("Matrices", sizeof(glm::mat4) * 2, glm::value_ptr(identity), sizeof(glm::mat4));
}

// Loads and sets up shaders
void MainGame::loadShaders() {
	ShaderManager::getInstance().loadShader("ADS", "..\\res\\ADS.vert", "..\\res\\ADS.frag");
	ShaderManager::getInstance().loadShader("glyphs", "..\\res\\glyphs.vert", "..\\res\\glyphs.frag");
	setActiveShader("ADS");

	// Bind UBO to Shader
	UBOManager::getInstance().bindUBOToShader("Matrices", ShaderManager::getInstance().getShader("ADS")->ID(), "Matrices");
}

// Sets up Camera
void MainGame::setupCamera() {
	myCamera.initCamera(glm::vec3(0, 0, -75), 100.0f,
		(float)_gameDisplay.getWidth() / _gameDisplay.getHeight(), 0.01f, 1000.0f);
}

// Loads and sets up the physics engine
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

// Initialises the player ship
void MainGame::initShip() {
	Entity shipEntity = coordinator.createEntity(); // Create an entity for the ship

	// Add the transform component to the ship entity
	coordinator.addComponent(shipEntity, Transform{
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
		});
	coordinator.addComponent(shipEntity, shipMesh); // Add the mesh component to the ship entity
	coordinator.addComponent(shipEntity, shipTexture); // Add the texture component to the ship entity

	shipTransform = &coordinator.getComponent<Transform>(shipEntity); // Store the transform component in the shipTransform pointer

	gameEntities.push_back(shipEntity); // Add the ship entity to the game entities vector
}

// Initialises an asteroid
void MainGame::initAsteroid() {
	Entity asteroidEntity = coordinator.createEntity(); // Create an entity for the asteroid

	AsteroidManager& asteroidManager = AsteroidManager::getInstance(); // Asteroid manager to randomise elements of spawning

	glm::vec3 randomisedPos = asteroidManager.randomiseAsteroidPos();
	glm::vec3 randomisedScale = asteroidManager.randomiseAsteroidScale();
	glm::vec3 randomisedForwardDirection = asteroidManager.randomiseAsteroidForwardDirection(randomisedPos);
	// Add the transform component to the asteroid entity (partially randomised)
	coordinator.addComponent(asteroidEntity, Transform{
		randomisedPos,
		glm::vec3(0.0f, 0.0f, 0.0f),
		randomisedScale,
		randomisedForwardDirection,
		});

	coordinator.addComponent(asteroidEntity, asteroidMesh); // Add the mesh component to the asteroid entity
	coordinator.addComponent(asteroidEntity, asteroidTexture); // Add the texture component to the asteroid entity

	gameEntities.push_back(asteroidEntity); // Add the asteroid entity to the game entities vector

	asteroids.push_back(asteroidEntity); // Store the asteroid entity in the asteroids vector (for referencing asteroids specifically)
}

// Main game loop
void MainGame::gameLoop() {
	while (_gameState != GameState::EXIT) {
		processInput();
		drawGame();
		if (_gameState != GameState::GAMEOVER) {
			calculateDeltaTime();
			updatePlayer(deltaTime);
			update(deltaTime);
		}
	}
}

// Processes input events
void MainGame::processInput()
{
	SDL_Event evnt;

	while (SDL_PollEvent(&evnt)) {
		if (evnt.type == SDL_QUIT) {
			_gameState = GameState::EXIT;
		}
	}

	// Keystates are used as they allow a key to be held down without a pause before repeating
	const Uint8* keystates = SDL_GetKeyboardState(nullptr);

	handleKeyPress(keystates);
}

// Handles key presses and applies physics to the ship
void MainGame::handleKeyPress(const Uint8* keystates) {
	if (!shipTransform) return; // Ensure ship exists before applying physics

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

// Firing lasers from the ship
void MainGame::fireLaser() {
	Entity laserEntity = coordinator.createEntity(); // Create an entity for the laser

	// Add the transform component to the laser entity (borrowing the ship's transform)
	coordinator.addComponent(laserEntity, Transform{
		shipTransform->position,
		shipTransform->rotation,
		glm::vec3(1.0f, 1.0f, 1.0f),
		shipTransform->forwardDirection
		});
	coordinator.addComponent(laserEntity, laserMesh); // Add the mesh component to the laser entity
	coordinator.addComponent(laserEntity, laserTexture); // Add the texture component to the laser entity
	
	lasers.push_back(laserEntity); // Store the laser entity in the lasers vector (for referencing lasers specifically)

	gameEntities.push_back(laserEntity); // Add the laser entity to the game entities vector
}

// Calculates delta time
void MainGame::calculateDeltaTime() {
	float currentFrameTime = SDL_GetTicks() / 1000.0f; // Get current time in seconds
	deltaTime = currentFrameTime - lastFrameTime;
	lastFrameTime = currentFrameTime;
}

// Update player-specific logic
void MainGame::updatePlayer(float deltaTime) {
	if (!shipTransform) return;

	shipTransform->position += shipTransform->velocity * deltaTime;

	// Code for wrapping the player around the screen if they leave it
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
}

// Update general game logic
void MainGame::update(float deltaTime) {
	// Timer for spawning new asteroids
	if (spawnDelay > 0.0f)
		spawnDelay -= deltaTime;

	// Timer for firing lasers
	if (fireDelay > 0.0f)
		fireDelay -= deltaTime;

	float spawnRate = 0.2f; // Spawn rate in seconds

	if (spawnDelay <= 0.0f) {
		initAsteroid(); // Spawn a new asteroid
		spawnDelay = spawnRate;
	}

	// Bounds for deleting asteroids and lasers if they go far off screen
	float deleteBoundsX = 50.0f;
	float deleteBoundsY = 40.0f;

	const float laserSpeed = 15.0f;

	for (auto& laser : lasers) {

		if (coordinator.hasComponent<Transform>(laser) == false) continue; // Check if the laser has a transform component
		auto* laserTransform = &coordinator.getComponent<Transform>(laser);

		laserTransform->position += laserTransform->forwardDirection * laserSpeed * deltaTime; // Move laser forward

		// Code for deleting lasers if they go far off screen
		if (laserTransform->position.x > deleteBoundsX || laserTransform->position.x < -deleteBoundsX ||
			laserTransform->position.y > deleteBoundsY || laserTransform->position.y < -deleteBoundsY) {
			coordinator.destroyEntity(laser);
			lasers.erase(std::remove(lasers.begin(), lasers.end(), laser), lasers.end());
		}
	}

	const float asteroidSpeed = 0.5f;

	for (auto& asteroid : asteroids) {

		if (coordinator.hasComponent<Transform>(asteroid) == false) continue; // Check if the asteroid has a transform component
		auto* asteroidTransform = &coordinator.getComponent<Transform>(asteroid);

		asteroidTransform->position += asteroidTransform->forwardDirection * asteroidSpeed * deltaTime; // Move asteroid forward

		if (checkCollisionAABB(shipTransform, asteroidTransform, asteroidTransform->scale * 0.75f, glm::vec3(1.0f, 1.0f, 1.0f))) {
			_gameState = GameState::GAMEOVER; // End game if player collides with an asteroid
		}

		// Code for deleting asteroids if they go far off screen
		if (asteroidTransform->position.x > deleteBoundsX || asteroidTransform->position.x < -deleteBoundsX ||
			asteroidTransform->position.y > deleteBoundsY || asteroidTransform->position.y < -deleteBoundsY) {
			coordinator.destroyEntity(asteroid);
			asteroids.erase(std::remove(asteroids.begin(), asteroids.end(), asteroid), asteroids.end());
		}

		for (auto& laser : lasers) {

			if (coordinator.hasComponent<Transform>(laser) == false) continue; // Check if the laser has a transform component
			auto* laserTransform = &coordinator.getComponent<Transform>(laser);

			if (checkCollisionAABB(asteroidTransform, laserTransform, asteroidTransform->scale, glm::vec3(1.0f, 1.0f, 1.0f))) {
				score += 100;
				coordinator.destroyEntity(asteroid); // Destroy the asteroid
				coordinator.destroyEntity(laser); // Destroy the laser
				asteroids.erase(std::remove(asteroids.begin(), asteroids.end(), asteroid), asteroids.end());
				lasers.erase(std::remove(lasers.begin(), lasers.end(), laser), lasers.end());
			}
		}
	}
}

// Sets the active shader for rendering
void MainGame::setActiveShader(const std::string& shaderTag) {
	if (ShaderManager::getInstance().getShader(shaderTag)) {
		activeShaderTag = shaderTag;
		std::cout << "Shader switched to: " << shaderTag << std::endl;
	}
	else {
		std::cerr << "Error: Shader not found - " << shaderTag << std::endl;
	}
}

// Clears the screen buffer
void MainGame::clearScreenBuffer()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set clear color to black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear colour and depth buffer - set colour to colour defined in glClearColor
}

// Renders game objects
void MainGame::renderGameObjects() {
	for (auto& entity : gameEntities) {
		// Get the entity's components
		auto& transform = coordinator.getComponent<Transform>(entity);
		auto& mesh = coordinator.getComponent<Mesh>(entity);
		auto& texture = coordinator.getComponent<Texture>(entity);

		// Checks to not repeat binding the same shader and texture
		Shader* currentShader = nullptr;
		GLuint currentTexture = 0;

		currentShader = ShaderManager::getInstance().getShader("ADS").get();

		currentShader->Bind();

		// Bind the correct texture
		if (texture.textureHandler != currentTexture) {
			textureSystem.Bind(texture.textureHandler);
			currentTexture = texture.textureHandler;
		}

		glm::mat4 model = transformSystem.update(entity);
		glm::mat4 view = myCamera.getView();
		glm::mat4 projection = myCamera.getProjection();

		UBOManager::getInstance().updateUBOData("Matrices", 0, glm::value_ptr(model), sizeof(glm::mat4));
		UBOManager::getInstance().updateUBOData("Matrices", sizeof(glm::mat4), glm::value_ptr(view), sizeof(glm::mat4));
		UBOManager::getInstance().updateUBOData("Matrices", sizeof(glm::mat4) * 2, glm::value_ptr(projection), sizeof(glm::mat4));

		meshSystem.render(mesh);
	}
}

// Draws the game
void MainGame::drawGame() {
	TextManager& textManager = TextManager::getInstance(); // Get the text manager
	clearScreenBuffer();
	if (_gameState == GameState::PLAY)
	{
		textManager.renderText(*ShaderManager::getInstance().getShader("glyphs").get(), std::to_string(score), 125.0f, 500.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f)); // Display the score
		renderGameObjects();
	}
	if (_gameState == GameState::GAMEOVER) 
	{
		// Display game over text, score, and exit message
		textManager.renderText(*ShaderManager::getInstance().getShader("glyphs").get(), "Game Over", 275.0f, 400.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		textManager.renderText(*ShaderManager::getInstance().getShader("glyphs").get(), std::to_string(score), 275.0f, 300.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
		textManager.renderText(*ShaderManager::getInstance().getShader("glyphs").get(), "Press ESC to exit", 275.0f, 200.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
	}
	_gameDisplay.swapBuffers();
}

