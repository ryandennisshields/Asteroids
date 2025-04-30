//
// This was used as a guide:
// https://austinmorlan.com/posts/entity_component_system/
//
#pragma once

#include "Types.h"
#include "ComponentArray.h"
#include <unordered_map>
#include <memory>

class ComponentManager
{
public:
	template<typename T>
	void registerComponent() {
		std::string typeName = typeid(T).name(); // Get the type name of the component

		if (componentTypes.find(typeName) != componentTypes.end()) {
			std::cerr << "Error: Component type already registered" << std::endl;
			return;
		}

		componentTypes.insert({ typeName, nextComponentType }); // Insert the component type and its ID into the map

		componentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() }); // Insert the component array into the map

		nextComponentType++; // Increment the component type ID
	}

	template<typename T>
	ComponentType getComponentType() {
		std::string typeName = typeid(T).name(); // Get the type name of the component

		if (componentTypes.find(typeName) == componentTypes.end()) {
			std::cerr << "Error: Component type not registered" << std::endl;
			return -1;
		}
		return componentTypes[typeName]; // Return the component type ID
	}

	template<typename T>
	void addComponent(Entity entity, T component) {
		getComponentArray<T>()->insertData(entity, component); // Insert the component into the component array
	}

	template<typename T>
	void removeComponent(Entity entity) {
		getComponentArray<T>()->removeData(entity); // Remove the component from the component array
	}

	template<typename T>
	T& getComponent(Entity entity) {
		return getComponentArray<T>()->getData(entity); // Get the component from the component array
	}

	void entityDestroyed(Entity entity);

private:
	std::unordered_map<std::string, ComponentType> componentTypes; // Map of component type to component type ID

	std::unordered_map<std::string, std::shared_ptr<IComponentArray>> componentArrays; // Map of component type to component array

	ComponentType nextComponentType{}; // Component type to be assigned to next registered component

	template<typename T>
	std::shared_ptr<ComponentArray<T>> getComponentArray()
	{
		std::string typeName = typeid(T).name(); // Get the type name of the component

		if (componentArrays.find(typeName) == componentArrays.end()) {
			std::cerr << "Error: Component type not registered: " << typeName << std::endl;
			return nullptr;
		}

		return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeName]); // Return the component array
	}
};