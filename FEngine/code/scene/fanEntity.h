#pragma once

#include "core/fanSignal.h"
#include "core/math/shapes/fanAABB.h"
#include "core/fanISerializable.h"

namespace fan
{
	namespace scene
	{
		class Component;

		//================================================================================================================================
		//================================================================================================================================
		class Entity : public fan::ISerializable {
		public:
			enum Flag {
				NONE = 0x00,
				NO_DELETE = 0x01,
				NOT_SAVED = 0x02,
			};

			fan::Signal<Component*> onComponentCreated;
			fan::Signal<Component*> onComponentDeleted;
			fan::Signal<Component*> onComponentModified;

			Entity(const std::string _name, Entity * _parent);
			~Entity();

			// Creates an instance of ComponentType, adds it to the entity and returns a pointer
			template<typename ComponentType>
			ComponentType* AddComponent();
			Component* AddComponent(const uint32_t _componentID);

			// Returns a pointer on the first instance of ComponentType in the entity, nullptr if none exists
			template<typename ComponentType>
			ComponentType* GetComponent();

			template<typename ComponentType>
			std::vector<ComponentType*> GetComponents();

			// Remove the component from the entity and deletes it
			bool DeleteComponent(const Component * _component);

			// Returns the component vector
			const std::vector<Component*> & GetComponents() const { return m_components; }

			//Getters
			std::string GetName() { return m_name; }
			const shape::AABB & GetAABB() const { return m_aabb; }
			void ComputeAABB();

			// Hierarchy
			Entity* GetParent() const { return m_parent; }
			const std::vector<Entity*>& GetChilds() const { return m_childs; }
			bool IsAncestorOf	( const Entity * _node) const;
			void RemoveChild	( const Entity * _child);
			bool HasChild		( const Entity * _child);
			void AddChild		( Entity * _child);
			void SetParent		( Entity * _parent);
			void InsertBelow	( Entity * _brother);


			// ISerializable
			void Load(std::istream& _in) override;
			void Save(std::ostream& _out) override;

			bool		HasFlag(const Flag _flag) const	{ return m_flags & _flag; }
			uint32_t	GetFlags() const				{ return m_flags; }
			void		SetFlags(const uint32_t _flags) { m_flags = _flags; }

		private:
			std::string				m_name;
			std::vector<Entity*>	m_childs;
			Entity *				m_parent;
			uint32_t				m_flags;

			shape::AABB m_aabb;

			std::vector<Component*> m_components;

			void AddComponent(scene::Component * _component);
			void OnComponentModified(scene::Component * _component);
			void OnComponentDeleted(scene::Component * _component);

		};


		//================================================================================================================================
		//================================================================================================================================
		template<typename ComponentType>
		ComponentType* Entity::AddComponent()
		{
			// Checks if ComponentType derivates from Component
			static_assert((std::is_base_of<Component, ComponentType>::value));

			ComponentType* component = new ComponentType();
			if (component->IsUnique() && GetComponent< ComponentType >() != nullptr) {
				delete(component);
				return nullptr;
			}

			AddComponent(component);

			return component;
		}

		//================================================================================================================================
		//================================================================================================================================
		template<typename ComponentType>
		ComponentType* Entity::GetComponent()
		{
			for (int componentIndex = 0; componentIndex < m_components.size(); componentIndex++) {
				Component* component = m_components[componentIndex];

				if (component->IsType<ComponentType>()) {
					return static_cast<ComponentType*>(component);
				}
			}
			return nullptr;
		}

		//================================================================================================================================
		//================================================================================================================================
		template<typename ComponentType>
		std::vector<ComponentType*> Entity::GetComponents()
		{
			std::vector<ComponentType*> componentTypeVector;
			for (int componentIndex = 0; componentIndex < m_components.size(); componentIndex++) {
				scene::Component* component = m_components[componentIndex];
				if (component->IsType<ComponentType>()) {
					componentTypeVector.push_back(static_cast<ComponentType*>(component));
				}
			}
			return componentTypeVector;
		}
	}
}