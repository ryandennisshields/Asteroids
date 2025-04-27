//
// This was used as a guide:
// https://austinmorlan.com/posts/entity_component_system/
//
#include <vector>
#include <iostream>
#include <algorithm>

class EntityManager 
{
public:
	using Entity = unsigned int; // Entities as unsigned int (for IDs)
	using Signature = unsigned long long; // Signature as unsigned long long (for bitmask)

	static EntityManager& getInstance();

	EntityManager();

	Entity createEntity();
	void destroyEntity(Entity entity);

	void setSignature(Entity entity, Signature signature);
	Signature getSignature(Entity entity);
private:
	std::vector<Entity> entities; // Vector of available entities

	std::vector<Signature> signatures; // Vector of entity signatures

	int maxEntities; // Maximum number of entitie
};