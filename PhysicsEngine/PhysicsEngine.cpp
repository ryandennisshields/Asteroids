#include "PhysicsEngine.h"
#include <iostream>

// Function implementation
extern "C" PHYSICS_API void HelloWorld() {
    std::cout << "Hello from Physics Engine DLL!" << std::endl;
}

extern "C" PHYSICS_API void setForwardDirection(GameObject* obj, glm::vec3 newForward) {
    // Access the Z-axis rotation (in radians)
    float angle = obj->transform->GetRot()->z;

    // Compute 2D forward direction based on rotation
    glm::vec3 forward(-sin(angle), cos(angle), 0.0f);

    obj->forwardDirection = glm::normalize(forward);
}

extern "C" PHYSICS_API void applyThrust(GameObject* obj, float thrustAmount) {
    if (!obj || !obj->transform || !obj->transform->GetRot()) {
        std::cerr << "[ERROR] applyThrust: Invalid transform or rotation pointer.\n";
        return;
    }

    float angle = -obj->transform->GetRot()->z + glm::pi<float>();

    glm::vec3 thrustDirection;
    thrustDirection.x = sin(angle);
    thrustDirection.y = cos(angle);
    thrustDirection.z = 0.0f;

    thrustDirection = glm::normalize(thrustDirection);

    //if (thrustAmount > 0.3f)
        //thrustAmount = 0.3f; // cap thrust

    //if (glm::abs(obj->velocity.x) <= 1.0f && glm::abs(obj->velocity.y) <= 1.0f)
        obj->velocity -= thrustDirection * thrustAmount; // cap velocity

    std::cout << "Velocity: " << obj->velocity.x << ", "
        << obj->velocity.y << ", " << obj->velocity.z << std::endl;
}

extern "C" PHYSICS_API void updatePhysics(GameObject* obj, float deltaTime) {
    if (!obj) return; // Safety check

    float dragFactor = 0.98f; // Apply drag each frame

    // Apply drag gradually over time
    obj->velocity *= dragFactor;

    // Ensure velocity approaches zero without stalling due to floating-point precision
    if (glm::length(obj->velocity) < 0.001f) {
        obj->velocity = glm::vec3(0.0f);
    }
}

extern "C" PHYSICS_API bool checkCollisionRadius(const GameObject* a, const GameObject* b, float radiusA, float radiusB) {
    glm::vec3 posA = a->transform->pos;
    glm::vec3 posB = b->transform->pos;
    float distance = glm::distance(posA, posB);
    if (distance < (radiusA + radiusB)) 
    {
        std::cout << "Radius Collision between " << a << " and " << b << " detected" << std::endl;
        return true;
    }
    else
    {
        return false;
    }
}

extern "C" PHYSICS_API bool checkCollisionAABB(const GameObject* a, const GameObject* b, const glm::vec3& halfExtentsA, const glm::vec3& halfExtentsB) {
    glm::vec3 posA = a->transform->pos;
    glm::vec3 posB = b->transform->pos;
    if (abs(posA.x - posB.x) < (halfExtentsA.x + halfExtentsB.x) &&
        abs(posA.y - posB.y) < (halfExtentsA.y + halfExtentsB.y) &&
        abs(posA.z - posB.z) < (halfExtentsA.z + halfExtentsB.z))
    { 
        std::cout << "AABB Collision between " << a << " and " << b << " detected" << std::endl;
        return true;
    }
    else 
    {
        return false;
    }
}