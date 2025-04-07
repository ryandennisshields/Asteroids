#include "PhysicsEngine.h"
#include <iostream>

// Function implementation
extern "C" PHYSICS_API void HelloWorld() {
    std::cout << "Hello from Physics Engine DLL!" << std::endl;
}

extern "C" PHYSICS_API void setForwardDirection(GameObject * obj, glm::vec3 newForward) {
    obj->forwardDirection = glm::normalize(newForward);
}

extern "C" PHYSICS_API void applyThrust(GameObject * obj, float thrustAmount) {
    obj->velocity += obj->forwardDirection * thrustAmount;
    std::cout << "Velocity: " << obj->velocity.x << ", " << obj->velocity.y << ", " << obj->velocity.z << std::endl;
}

extern "C" PHYSICS_API void updatePhysics(GameObject * obj, float deltaTime) {
    float dragFactor = 0.99f;
    obj->velocity *= dragFactor;
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
    glm::vec3 posA = a->transform->GetPos();
    glm::vec3 posB = b->transform->GetPos();
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