//
// This was used as a guide:
// https://austinmorlan.com/posts/entity_component_system/
//
#pragma once

#include "Types.h"
#include "System.h"
#include <unordered_map>
#include <memory>

class SystemManager
{
public:
	template<typename T>
	std::shared_ptr<T> registerSystem()
	{
		std::string typeName = typeid(T).name(); // Get the type name of the system

		if (systems.find(typeName) != systems.end()) {
			std::cerr << "Error: System already registered" << std::endl;
			return nullptr;
		}

		auto system = std::make_shared<T>(); // Create a new system of type T
		systems.insert({ typeName, system }); // Insert the system into the map
		return system; // Return the system
	}

	template<typename T>
	void setSignature(Signature signature)
	{
		std::string typeName = typeid(T).name(); // Get the type name of the system

		if (systems.find(typeName) == systems.end()) {
			std::cerr << "Error: System not registered" << std::endl;
			return;
		}

		signatures.insert({ typeName, signature }); // Insert the signature into the map
	}

	void entityDestroyed(Entity entity);

	void entitySignatureChanged(Entity entity, Signature entitySignature);

private:
	std::unordered_map<std::string, Signature> signatures; // Map of system type to signature

	std::unordered_map<std::string, std::shared_ptr<System>> systems; // Map of system type to system
};