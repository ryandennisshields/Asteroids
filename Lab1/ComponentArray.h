#pragma once

#include "Types.h"
#include <array>
#include <unordered_map>

class IComponentArray
{
public:
	virtual ~IComponentArray() = default; // Clean up
	virtual void entityDestroyed(Entity entity) = 0; // Will be used to tell Component Array that an entity has been destroyed
};

template<typename T>
class ComponentArray : public IComponentArray
{
public:
	void insertData(Entity entity, T component) {
		if (entityToIndexMap.find(entity) != entityToIndexMap.end()) {
			throw std::runtime_error("Error: Entity already has that component");
		}

		size_t newIndex = size; // Get current size as new index entry
		entityToIndexMap[entity] = newIndex;  // Map entity to index (of components)
		indexToEntityMap[newIndex] = entity;  // Map index to entity
		componentArray[newIndex] = component; // Insert component at new index (so the entity is mapped to an actual component)
		size++; // Increment size
	}

	void removeData(Entity entity) {
		if (entityToIndexMap.find(entity) == entityToIndexMap.end()) {
			throw std::runtime_error("Error: Entity does not have that component");
		}

		// Copy element at the end of the vector into the deleted entity's place
		size_t indexOfRemovedEntity = entityToIndexMap[entity]; // Get index of removed entity
		size_t indexOfLastElement = size - 1; // Get last element index
		componentArray[indexOfRemovedEntity] = componentArray[indexOfLastElement]; // Move last element to the removed element's index

		// Update the map to point to the newly moved spot
		Entity entityOfLastElement = indexToEntityMap[indexOfLastElement]; // Get last element entity
		entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity; // Update the entity to index map
		indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement; // Update the index to entity map

		// Finally, remove the entity and its index from the map
		entityToIndexMap.erase(entity); // Remove the entity from the map
		indexToEntityMap.erase(indexOfLastElement); // Remove the last element from the map

		size--; // Decrement size
	}

	T& getData(Entity entity) {
		if (entityToIndexMap.find(entity) == entityToIndexMap.end()) {
			throw std::runtime_error("Error: Entity daoes not have that component");
		}
		return componentArray[entityToIndexMap[entity]]; // Return the component at the entity's index
	}

	void entityDestroyed(Entity entity) override {
		if (entityToIndexMap.find(entity) != entityToIndexMap.end()) {
			removeData(entity); // Remove the entity from the component array
		}
	}

private:
	std::array<T, maxEntities> componentArray; // Array of components

	std::unordered_map<Entity, size_t> entityToIndexMap; // Map of entity to index

	std::unordered_map<size_t, Entity> indexToEntityMap; // Map of index to entity

	size_t size; // Size of the component array
};