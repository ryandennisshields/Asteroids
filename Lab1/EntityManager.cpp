//
// This was used as a guide:
// https://austinmorlan.com/posts/entity_component_system/
//
#include "EntityManager.h"

EntityManager& EntityManager::getInstance()
{
	static EntityManager instance; // Create a static instance of EntityManager
	return instance; // Return the instance
}

EntityManager::EntityManager()
	: maxEntities(100) // Set the maximum number of entities
{
	for (Entity entity = 0; entity < maxEntities; ++entity) { // Preallocate 200 entities
		entities.push_back(entity); // Add the entity to the vector
	}
}

EntityManager::Entity EntityManager::createEntity()
{
	if (entities.size() >= maxEntities) {
		std::cerr << "Error: Maximum entity limit reached" << std::endl;
		return Entity(); // Return an empty entity
	}

	Entity id = entities.front(); // Get an entity from the front of the vector
	entities.erase(entities.begin()); // Remove oldest entity from the vector

	std::cout << "Entity created with ID: " << id << std::endl;

	return id; // Return the frontmost entity id in the vector
}

void EntityManager::destroyEntity(Entity entity)
{
	if (entity > entities.size()) {
		std::cerr << "Error: Entity ID out of range" << std::endl;
		return;
	}

	signatures.erase(signatures.begin() + entity); // Remove the signature for the entity
	entities.push_back(entity); // Add the entity back to the vector
}

void EntityManager::setSignature(Entity entity, Signature signature)
{
	if (entity > signatures.size()) {
		std::cerr << "Error: Entity ID out of range" << std::endl;
		return;
	}

	signatures[entity] = signature; // Set the signature for the entity
}

EntityManager::Signature EntityManager::getSignature(Entity entity)
{
	if (entity > signatures.size()) {
		std::cerr << "Error: Entity ID out of range" << std::endl;
		return Signature(); // Return an empty signature
	}

	return signatures[entity]; // Return the signature for the entity
}