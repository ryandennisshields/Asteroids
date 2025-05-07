//
// This was used as a guide:
// https://austinmorlan.com/posts/entity_component_system/
//
#include "EntityManager.h"

EntityManager::EntityManager()
{
	for (Entity entity = 0; entity < maxEntities; ++entity) { // Preallocate 200 availableEntities
		availableEntities.push(entity); // Add the entity to the vector
	}
}

Entity EntityManager::createEntity()
{
	if (livingEntityCount > maxEntities) {
		std::cerr << "Error: Maximum entity limit reached" << std::endl;
		return Entity(); // Return an empty entity
	}

	Entity id = availableEntities.front(); // Get an entity from the front of the vector
	availableEntities.pop(); // Remove oldest entity from the vector
	livingEntityCount++; // Increment the living entity count

	return id; // Return the frontmost entity id in the vector
}

void EntityManager::destroyEntity(Entity entity)
{
	if (entity > maxEntities) {
		std::cerr << "Error: Entity ID out of range" << std::endl;
		return;
	}

	signatures[entity].reset(); // Reset the signature for the entity

	availableEntities.push(entity); // Add the entity back to the vector
	livingEntityCount--; // Decrement the living entity count
}

void EntityManager::setSignature(Entity entity, Signature signature)
{
	if (entity > maxEntities) {
		std::cerr << "Error: Entity ID out of range" << std::endl;
		return;
	}

	signatures[entity] = signature; // Set the signature for the entity
}

Signature EntityManager::getSignature(Entity entity)
{
	if (entity > maxEntities) {
		std::cerr << "Error: Entity ID out of range" << std::endl;
		return Signature(); // Return an empty signature
	}

	return signatures[entity]; // Return the signature for the entity
}