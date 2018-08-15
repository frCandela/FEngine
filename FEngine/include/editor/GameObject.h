#pragma once

#include <vector>
#include <iostream>
#include <exception>
#include <cassert>
#include <type_traits>

#include "util/Signal.h"
#include "Component.h"
class Component;

// Base class for all entities in the engine
class GameObject
{
public:

	GameObject(std::string nameStr)	: 
		m_name(nameStr)
	{

	}

	~GameObject()
	{
		for (Component* component : m_components)
			delete(component);
	}

	// Creates an instance of ComponentType, adds it to the GameObject and returns a pointer
	template<typename ComponentType>
	ComponentType* AddComponent();

	// Returns a pointer on the first instance of ComponentType in the GameObject, nullptr if none exists
	template<typename ComponentType>
	ComponentType* GetComponent();

	// Returns a Vector of pointers the instance of ComponentType in the GameObject
	template<typename ComponentType>
	std::vector<ComponentType*> GetComponents();

	// Remove the component from the GameObject and deletes it
	bool DeleteComponent(Component* component);

	// Returns the number of component of the gameobject
	size_t ComponentsCount() { return m_components.size(); }

	// Returns the component at the given index
	Component* GetComponent(size_t index) { return m_components[index]; }

	// Returns the component vector
	std::vector<Component*> GetComponents() { return m_components; }

	//Getters
	inline std::string GetName() { return m_name; }

	Signal<GameObject*, Component*> onComponentDeleted;

private:
	std::vector<Component*> m_components;
	std::string m_name;
};

template<typename ComponentType>
ComponentType* GameObject::AddComponent()
{
	ComponentType* componentType = new ComponentType();

	componentType->m_pGameobject = this;

	// Checks if ComponentType derivates from Component
	assert( (std::is_base_of<Component, ComponentType>::value ));

	// Checks if ComponentType is unique and doesn't isn't already added to the GameObject
	//assert(!dynamic_cast<Component*>(componentType)->IsUnique() || GetComponent<ComponentType>() == nullptr);
	if (dynamic_cast<Component*>(componentType)->IsUnique() && GetComponent<ComponentType>() != nullptr)
		return nullptr;

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
