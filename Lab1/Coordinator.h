//
// This was used as a guide:
// https://austinmorlan.com/posts/entity_component_system/
//
#pragma once

#include "Types.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

class Coordinator 
{
public:
	Coordinator() = default;

	void init();

	Entity createEntity();
	void destroyEntity(Entity entity);

	template<typename T>
	void registerComponent()
	{
		componentManager->registerComponent<T>(); // Tell the component manager to register the component
	}
	template<typename T>
	void addComponent(Entity entity, T component)
	{
		componentManager->addComponent<T>(entity, component); // Tell the component manager to add the component to the entity

		auto signature = entityManager->getSignature(entity); // Get the entity's signature
		signature.set(componentManager->getComponentType<T>(), true); // Set the component bit in the signature
		entityManager->setSignature(entity, signature); // Update the entity's signature

		systemManager->entitySignatureChanged(entity, signature); // Notify the system manager that the entity's signature has changed
	}

	template<typename T>
	void removeComponent(Entity entity)
	{
		componentManager->removeComponent<T>(entity); // Tell the component manager to remove the component from the entity

		auto signature = entityManager->getSignature(entity); // Get the entity's signature
		signature.set(componentManager->getComponentType<T>(), false); // Clear the component bit in the signature
		entityManager->setSignature(entity, signature); // Update the entity's signature

		systemManager->entitySignatureChanged(entity, signature); // Notify the system manager that the entity's signature has changed
	}

	template<typename T>
	T& getComponent(Entity entity)
	{
		return componentManager->getComponent<T>(entity); // Tell the component manager to get the component from the entity
	}

	template<typename T>
	bool hasComponent(Entity entity) {
		return componentManager->hasComponent<T>(entity);
	}

	template<typename T>
	ComponentType getComponentType()
	{
		return componentManager->getComponentType<T>(); // Tell the component manager to get the component type
	}


	template<typename T>
	std::shared_ptr<T> registerSystem()
	{
		return systemManager->registerSystem<T>(); // Tell the system manager to register the system
	}

	template<typename T>
	void setSystemSignature(Signature signature)
	{
		systemManager->setSignature<T>(signature); // Tell the system manager to set the system signature
	}

	std::unique_ptr<EntityManager> entityManager; // Entity manager instance
	std::unique_ptr<ComponentManager> componentManager; // Component manager instance
	std::unique_ptr<SystemManager> systemManager; // System manager instance
};

extern std::vector<Entity> gameEntities; // Entities for MainGame to use
extern Coordinator coordinator; // Declare the coordinator instance