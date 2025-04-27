//
// This was used as a guide:
// https://austinmorlan.com/posts/entity_component_system/
//
#include "EntityManager.h"

class IComponentArray 
{
public:
	virtual ~IComponentArray() = default; // Clean up
	virtual void entityDestroyed(EntityManager::Entity entity) = 0; // Will be used to tell Component Array that an entity has been destroyed
};

template <typename T> 
class ComponentArray : public IComponentArray
{
public:
	void insertData(EntityManager::Entity, T component);
	void removeData(EntityManager::Entity entity);

	T& getData(EntityManager::Entity entity);

	void entityDestroyed(EntityManager::Entity entity) override;

private:
	std::vector<T, EntityManager::maxEntities> componentArray; // Vector of components

	std::unordered_map<EntityManager::Entity, size_t> entityToIndexMap; // Map of entity to index

	std::unordered_map<size_t, EntityManager::Entity> indexToEntityMap; // Map of index to entity

	size_t size; // Size of the component array
};

class ComponentManager
{

};