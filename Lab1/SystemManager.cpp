//
// This was used as a guide:
// https://austinmorlan.com/posts/entity_component_system/
//
#include "SystemManager.h"

void SystemManager::entityDestroyed(Entity entity)
{
	// Erase a destoyed entity from all system vectors
	for (auto const& pair : systems) { // Iterate through the systems
		auto const& system = pair.second; // Get the system
		system->entities.erase(entity); // Remove the entity from the system
	}
}

void SystemManager::entitySignatureChanged(Entity entity, Signature entitySignature)
{
	for (auto const& pair : systems) { // Iterate through the systems
		auto const& type = pair.first; // Get the system type
		auto const& system = pair.second; // Get the system
		auto const& systemSignature = signatures[type]; // Get the system signature

		if ((entitySignature & systemSignature) == systemSignature) { // Check if the entity signature matches the system signature
			// MIGHT CAUSE ISSUES
			system.get()->entities.insert(entity); // Add the entity to the system
		}
		else {
			system.get()->entities.erase(entity);// Remove the entity from the system
		}
	}
}