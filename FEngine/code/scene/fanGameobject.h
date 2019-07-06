#pragma once

#include "util/fanSignal.h"

namespace scene
{
	class Component;

	class Gameobject {
	public:

		util::Signal<Component*> onComponentCreated;
		util::Signal<Component*> onComponentDeleted;
		util::Signal<Component*> onComponentModified;

		Gameobject(const std::string _name);
		~Gameobject();

		// Creates an instance of ComponentType, adds it to the GameObject and returns a pointer
		template<typename ComponentType>
		ComponentType* AddComponent();

		// Returns a pointer on the first instance of ComponentType in the GameObject, nullptr if none exists
		template<typename ComponentType>
		ComponentType* GetComponent();

		template<typename ComponentType>
		std::vector<ComponentType*> GetComponents();

		// Remove the component from the GameObject and deletes it
		bool DeleteComponent( const Component * _component );

		// Returns the component vector
		const std::vector<Component*> & GetComponents() const { return m_components; }

		//Getters
		std::string GetName() { return m_name; }

	private:
		std::string m_name;
		std::vector<Component*> m_components;
	};


	//================================================================================================================================
	//================================================================================================================================
	template<typename ComponentType>
	ComponentType* Gameobject::AddComponent()
	{
		ComponentType* componentType = new ComponentType();

		componentType->m_gameobject = this;

		// Checks if ComponentType derivates from Component
		assert((std::is_base_of<Component, ComponentType>::value));

		// Checks if ComponentType is unique and doesn't isn't already added to the GameObject	
		if (componentType->IsUnique() && GetComponent<ComponentType>() != nullptr)
			return nullptr;

		m_components.push_back(componentType);

		onComponentCreated.Emmit(componentType);
		onComponentModified.Emmit(componentType);

		return componentType;
	}

	//================================================================================================================================
	//================================================================================================================================
	template<typename ComponentType>
	ComponentType* Gameobject::GetComponent()
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

	//================================================================================================================================
	//================================================================================================================================
	template<typename ComponentType>
	std::vector<ComponentType*> Gameobject::GetComponents()
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
}