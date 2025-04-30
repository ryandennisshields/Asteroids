//
// This was used as a guide:
// https://austinmorlan.com/posts/entity_component_system/
//
#include "ComponentManager.h"

void ComponentManager::entityDestroyed(Entity entity) {
	// Notify component vector that an entity has been destroyed
	// If they hold a component for an entity, they will remove it
	for (auto const& pair : componentArrays) { // Iterate through the component arrays
		auto const& component = pair.second; // Get the component array
		component->entityDestroyed(entity); // Call entityDestroyed on the component array
	}
}