#pragma once  

#include "Types.h"
#include <queue>
#include <array>
#include <iostream>

class EntityManager  
{  
public:  
   EntityManager();  

   Entity createEntity();  
   void destroyEntity(Entity entity);  

   void setSignature(Entity entity, Signature signature);  
   Signature getSignature(Entity entity); 

private:  
   std::queue<Entity> availableEntities{}; // Vector of available entities  

   std::array<Signature, maxEntities> signatures; // Array of entity signatures 

   uint32_t livingEntityCount{}; // Count of living entities
};