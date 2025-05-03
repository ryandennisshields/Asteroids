#include "PhysicsEngine.h"

extern "C" PHYSICS_API void setForwardDirection(Transform* transform, glm::vec3 newForward) {
    // Access the Z-axis rotation (in radians)
	float angle = transform->rotation.z;

    // Compute 2D forward direction based on rotation
    glm::vec3 forward(-sin(angle), cos(angle), 0.0f);

    transform->forwardDirection = glm::normalize(forward);
}

extern "C" PHYSICS_API void applyThrust(Transform* transform, float thrustAmount) {

    if (!transform || !transform->rotation.z) {
        std::cerr << "[ERROR] applyThrust: Invalid transform or rotation pointer.\n";
        return;
    }

    float angle = -transform->rotation.z + glm::pi<float>();

    glm::vec3 thrustDirection;
    thrustDirection.x = sin(angle);
    thrustDirection.y = cos(angle);
    thrustDirection.z = 0.0f;

    thrustDirection = glm::normalize(thrustDirection);

    //if (thrustAmount > 0.3f)
        //thrustAmount = 0.3f; // cap thrust

    //if (glm::abs(obj->velocity.x) <= 1.0f && glm::abs(obj->velocity.y) <= 1.0f)
        transform->velocity -= thrustDirection * thrustAmount; // cap velocity

    std::cout << "Velocity: " << transform->velocity.x << ", "
        << transform->velocity.y << ", " << transform->velocity.z << std::endl;
}

extern "C" PHYSICS_API void updatePhysics(Transform* transform, float deltaTime) {
    if (!transform) return; // Safety check

    float dragFactor = 0.98f; // Apply drag each frame

    // Apply drag gradually over time
    transform->velocity *= dragFactor;

    // Ensure velocity approaches zero without stalling due to floating-point precision
    if (glm::length(transform->velocity) < 0.001f) {
        transform->velocity = glm::vec3(0.0f);
    }
}

extern "C" PHYSICS_API bool checkCollisionRadius(const Transform* a, const Transform* b, float radiusA, float radiusB) {
    glm::vec3 posA = a->position;
    glm::vec3 posB = b->position;
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

extern "C" PHYSICS_API bool checkCollisionAABB(const Transform* a, const Transform* b, const glm::vec3& halfExtentsA, const glm::vec3& halfExtentsB) {
    glm::vec3 posA = a->position;
    glm::vec3 posB = b->position;
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