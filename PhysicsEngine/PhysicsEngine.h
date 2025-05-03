#pragma once

#ifdef PHYSICSENGINE_EXPORTS
#define PHYSICS_API __declspec(dllexport)
#else
#define PHYSICS_API __declspec(dllimport)
#endif

#include "Transform.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>

// Function to set the forward direction of an object
extern "C" PHYSICS_API void setForwardDirection(Transform* transform, glm::vec3 newForward);

// Function to apply thrust to an object
extern "C" PHYSICS_API void applyThrust(Transform* transform, float thrustAmount);

// Function to update physics (apply drag, etc.)
extern "C" PHYSICS_API void updatePhysics(Transform* transform, float deltaTime);

extern "C" PHYSICS_API bool checkCollisionRadius(const Transform* a, const Transform* b, float radiusA, float radiusB);

extern "C" PHYSICS_API bool checkCollisionAABB(const Transform* a, const Transform* b, const glm::vec3& halfExtentsA, const glm::vec3& halfExtentsB);