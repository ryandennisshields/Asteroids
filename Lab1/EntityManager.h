#pragma once  

#include "Types.h"
#include <queue>
#include <array>
#include <iostream>

class EntityManager  
{  
public:  
   EntityManager();  

   Entity createEntity(); // Create an entity
   void destroyEntity(Entity entity); // Destroy an entity

   void setSignature(Entity entity, Signature signature); // Sets signatures for an entity
   Signature getSignature(Entity entity); // Gets an entity's signature

private:  
   std::queue<Entity> availableEntities{}; // Vector of available entities  

   std::array<Signature, maxEntities> signatures; // Array of entity signatures (stores what components an entity has)

   uint32_t livingEntityCount{}; // Count of living entities
};