#pragma once
#include <SDL\SDL.h>
#include <GL/glew.h>
#include <vector>
#include <algorithm>
#include <windows.h>  // Required for LoadLibrary & GetProcAddress
#include "ShaderManager.h"
#include "TransformManager.h"
#include "TextManager.h"
#include "UBOManager.h"
#include "AsteroidManager.h"
#include "Mesh.h"
#include "Texture.h"
#include "transform.h"
#include "DisplayFacade.h" 
#include "GameObject.h"

enum class GameState{PLAY, EXIT, GAMEOVER};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();

private:

	void setActiveShader(const std::string& shaderTag);
	void initSystems();
	void processInput();
	void fireLaser();
	void gameLoop();
	void drawGame();
	void clearScreenBuffer();
	void calculateDeltaTime();
	void loadMeshes();
	void loadTextures();
	void setupUBOs();
	void loadShaders();
	void setupCamera();
	void renderGameObjects();
	void renderPlayer();
	void setupTransforms();
	void createShip();
	void createAsteroid();
	void loadPhysicsEngine();
	void loadPhysicsEngineUnsafe();
	void handleKeyPress(const Uint8* keystate);
	void initShip();
	void initAsteroid();
	void updatePlayer(float deltaTime);
	void updateMovement(float deltaTime);
	float getRefreshRate();

	DisplayFacade _gameDisplay;
	GameState _gameState;
	Mesh shipMesh;
	Mesh asteroidMesh;
	Mesh laserMesh;
	Camera myCamera;
	Texture texture; 
	Transform transform;
	Transform shipTransform;

	int score = 0;
	float counter;
	float fireDelay;

	float deltaTime = 0.0f;
	float lastFrameTime = 0.0f;
	float accumulator = 0.0f;
	mutable float fixedTimeStep; // 60 physics updates per second

	GameObject* ship; // Store a pointer to the ship
	GameObject* asteroid;
	GameObject* laser;
	std::vector<GameObject> asteroids; // Store all asteroids
	std::vector<GameObject> lasers;
	std::string activeShaderTag; // Track the active shader

	// Function pointers for physics engine functions
	void (*setForwardDirection)(GameObject*, glm::vec3) = nullptr;
	void (*applyThrust)(GameObject*, float) = nullptr;
	void (*updatePhysics)(GameObject*, float) = nullptr;

	bool (*checkCollisionRadius)(const GameObject*, const GameObject*, float, float) = nullptr;
	bool (*checkCollisionAABB)(const GameObject*, const GameObject*, const glm::vec3&, const glm::vec3&) = nullptr;

	// Function pointer for HelloWorld test function
	void (*HelloWorld)() = nullptr;
};

