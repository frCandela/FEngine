#pragma once

#include <vector>
#include <iostream>
#include <exception>
#include <cassert>
#include <type_traits>

#include "Component.h"
class Component;

#include "Transform.h"

///Base class for all entities in the engine
class GameObject
{
public:

	/// Creates an instance of ComponentType, adds it to the GameObject and returns a pointer
	template<typename ComponentType>
	ComponentType* AddComponent();

	/// Returns a pointer on the first instance of ComponentType in the GameObject, nullptr if none exists
	template<typename ComponentType>
	ComponentType* GetComponent();

	/// Returns a Vector of pointers the instance of ComponentType in the GameObject
	template<typename ComponentType>
	std::vector<ComponentType*> GetComponents();

	/// Remove the component from the GameObject and deletes it
	bool DeleteComponent(Component* component);


	Transform transform;

private:
	std::vector<Component*> m_components;
};

template<typename ComponentType>
ComponentType* GameObject::AddComponent()
{
	ComponentType* componentType = new ComponentType();

	componentType->m_pGameobject = this;


	// Checks if ComponentType derivates from Component
	assert( (std::is_base_of<Component, ComponentType>::value ));

	// Checks if ComponentType is unique and doesn't isn't already added to the GameObject
	assert(!dynamic_cast<Component*>(componentType)->IsUnique() || GetComponent<ComponentType>() == nullptr);

	m_components.push_back(componentType);

	return componentType;
}

template<typename ComponentType>
ComponentType* GameObject::GetComponent()
{
	ComponentType * componentType;
	for (Component* component : m_components)
	{
		componentType = dynamic_cast<ComponentType*>(component);
		if (componentType != nullptr)
			return componentType;
	}
	return nullptr;
}

template<typename ComponentType>
std::vector<ComponentType*> GameObject::GetComponents()
{
	std::vector<ComponentType*> componentTypeVector;
	ComponentType* componentType;
	for (Component* component : m_components)
	{
		componentType = dynamic_cast<ComponentType*>(component);
		if (componentType != nullptr)
			componentTypeVector.push_back(componentType);
	}
	return componentTypeVector;
}
