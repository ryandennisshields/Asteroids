#pragma once

#ifdef PHYSICSENGINE_EXPORTS
#define PHYSICS_API __declspec(dllexport)
#else
#define PHYSICS_API __declspec(dllimport)
#endif

#include "GameObject.h"
#include <glm/glm.hpp>

// Function to set the forward direction of an object
extern "C" PHYSICS_API void setForwardDirection(GameObject * obj, glm::vec3 newForward);

// Function to apply thrust to an object
extern "C" PHYSICS_API void applyThrust(GameObject * obj, float thrustAmount);

// Function to update physics (apply drag, etc.)
extern "C" PHYSICS_API void updatePhysics(GameObject * obj, float deltaTime);

extern "C" PHYSICS_API bool checkCollisionRadius(const GameObject* a, const GameObject* b, float radiusA, float radiusB);

extern "C" PHYSICS_API bool checkCollisionAABB(const GameObject* a, const GameObject* b, const glm::vec3& halfExtentsA, const glm::vec3& halfExtentsB);