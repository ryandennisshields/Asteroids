#pragma once

#include <bitset>

using Entity = std::uint32_t; // Entities as uint32_t
const Entity maxEntities = 100; // Maximum number of entities 
using ComponentType = std::uint8_t; // Component type as uint8_t
const ComponentType maxComponents = 5; // Maximum number of components
using Signature = std::bitset<maxComponents>; // Signature as bitset