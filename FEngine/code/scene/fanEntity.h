#pragma once

#include "core/fanSignal.h"
#include "core/math/shapes/fanAABB.h"
#include "core/fanISerializable.h"

namespace fan
{
	namespace scene
	{
		class Component;
		class Scene;

		//================================================================================================================================
		//================================================================================================================================
		class Entity : public fan::ISerializable {
		public:
			enum Flag {
				NONE = 0x00,
				NO_DELETE = 0x01,
				NOT_SAVED = 0x02,
			};

			Entity(const std::string _name, Entity * _parent);
			~Entity();

			void OnGui();
			
			template<typename ComponentType>
			ComponentType* AddComponent();// Creates an instance of ComponentType, adds it to the entity and returns a pointer
			Component* AddComponent(const uint32_t _componentID);			
			template<typename ComponentType>
			ComponentType* GetComponent();// Returns a pointer on the first instance of ComponentType in the entity, nullptr if none exists
			template<typename ComponentType>
			std::vector<ComponentType*> GetComponents();			
			bool DeleteComponent(const Component * _component);// Remove the component from the entity and deletes it			
			const std::vector<Component*> & GetComponents() const { return m_components; }// Returns the component vector

			// Getters
			std::string		GetName() const { return m_name; }
			void			SetName(const std::string _newName) { m_name = _newName; }
			scene::Scene *	GetScene() const { return m_scene; }
			void			SetScene(Scene * _scene) { m_scene = _scene;  }

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
			bool LoadEntity( std::istream& _in );
			bool Save(std::ostream& _out, const int _indentLevel) const override;

			bool		HasFlag(const Flag _flag) const	{ return m_flags & _flag; }
			uint32_t	GetFlags() const				{ return m_flags; }
			void		SetFlags(const uint32_t _flags) { m_flags = _flags; }

		private:
			std::string				m_name;
			std::vector<Entity*>	m_childs;
			Entity *				m_parent;
			uint32_t				m_flags;
			shape::AABB				m_aabb;
			std::vector<Component*> m_components;
			scene::Scene *			m_scene;

			bool m_computeAABB = true;

			bool Load(std::istream& _in) override;
			void AddComponent(scene::Component * _component);
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
				delete(  static_cast<Component*>( component ) );
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