#include "ComponentManager.h"

template <typename T>  
void ComponentArray<T>::insertData(EntityManager::Entity entity, T component) {  
   if (entityToIndexMap.find(entity) != entityToIndexMap.end()) {  
       std::cerr << "Error: Entity already has that component" << std::endl;  
	   return;
   }

   size_t newIndex = size; // Get current size as new index entry
   entityToIndexMap[entity] = newIndex;  // Map entity to index (of components)
   indexToEntityMap[newIndex] = entity;  // Map index to entity
   componentArray[newIndex] = component; // Insert component at new index (so the entity is mapped to an actual component)
   size++; // Increment size
}

template <typename T>
void ComponentArray<T>::removeData(EntityManager::Entity entity) {
	if (entityToIndexMap.find(entity) == entityToIndexMap.end()) {
		std::cerr << "Error: Entity does not have that component" << std::endl;
		return;
	}

	// Copy element at the end of the vector into the deleted entity's place
	size_t indexOfRemovedEntity = entityToIndexMap[entity]; // Get index of removed entity
	size_t indexOfLastElement = size - 1; // Get last element index
	componentArray[indexOfRemovedEntity] = componentArray[indexOfLastElement]; // Move last element to the removed element's index

	// Update the map to point to the newly moved spot
	EntityManager::Entity entityOfLastElement = indexToEntityMap[indexOfLastElement]; // Get last element entity
	entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity; // Update the entity to index map
	indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement; // Update the index to entity map

	// Finally, remove the entity and its index from the map
	entityToIndexMap.erase(entity); // Remove the entity from the map
	indexToEntityMap.erase(indexOfLastElement); // Remove the last element from the map

	size--; // Decrement size
}

template <typename T>
T& ComponentArray<T>::getData(EntityManager::Entity entity) {
	if (entityToIndexMap.find(entity) == entityToIndexMap.end()) {
		std::cerr << "Error: Entity does not have that component" << std::endl;
		return;
	}
	return componentArray[entityToIndexMap[entity]]; // Return the component at the entity's index
}

template <typename T>
void ComponentArray<T>::entityDestroyed(EntityManager::Entity entity) {
	if (entityToIndexMap.find(entity) != entityToIndexMap.end()) {
		removeData(entity); // Remove the entity from the component array
	}
}