#include "Coordinator.h"

void Coordinator::init()
{
   entityManager = std::make_unique<EntityManager>();
   componentManager = std::make_unique<ComponentManager>();
   systemManager = std::make_unique<SystemManager>();
}

Entity Coordinator::createEntity()
{
	return entityManager->createEntity(); // Tell the entity manager to create an entity
}

void Coordinator::destroyEntity(Entity entity)
{
	entityManager->destroyEntity(entity); // Tell the entity manager to destroy the entity
	componentManager->entityDestroyed(entity); // Notify the component manager that the entity has been destroyed
	systemManager->entityDestroyed(entity); // Notify the system manager that the entity has been destroyed
}

std::vector<Entity> gameEntities(maxEntities); // Vector to hold game entities
Coordinator coordinator; // Coordinator instance
