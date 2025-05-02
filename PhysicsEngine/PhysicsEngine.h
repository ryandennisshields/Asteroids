#pragma once

#ifdef PHYSICSENGINE_EXPORTS
#define PHYSICS_API __declspec(dllexport)
#else
#define PHYSICS_API __declspec(dllimport)
#endif

#include "Coordinator.h"
#include "TransformSystem.h"
#include <glm/glm.hpp>

// Function to set the forward direction of an object
extern "C" PHYSICS_API void setForwardDirection(const Entity* obj, glm::vec3 newForward);

// Function to apply thrust to an object
extern "C" PHYSICS_API void applyThrust(const Entity* obj, float thrustAmount);

// Function to update physics (apply drag, etc.)
extern "C" PHYSICS_API void updatePhysics(const Entity* obj, float deltaTime);

extern "C" PHYSICS_API bool checkCollisionRadius(const Entity* a, const Entity* b, float radiusA, float radiusB);

extern "C" PHYSICS_API bool checkCollisionAABB(const Entity* a, const Entity* b, const glm::vec3& halfExtentsA, const glm::vec3& halfExtentsB);