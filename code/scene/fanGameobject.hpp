#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "core/fanSignal.hpp"
#include "core/math/shapes/fanAABB.hpp"
#include "core/resources/fanResource.hpp"
#include "game/fanGameSerializable.hpp"
#include "scene/ecs/fanECSManager.hpp"	
#include "scene/fanScene.hpp"

namespace fan
{
	class Component;
	class Transform;

	//================================================================================================================================
	//================================================================================================================================
	class Gameobject : public Resource, public ISerializable
	{
	public:
		using Flag = ecsFlags::Flag;
		using EditorFlag = ecsEditorFlags::Flag;

		static Signal< uint64_t, Gameobject* > s_setIDfailed;

		Gameobject( const std::string _name, Gameobject* _parent, Scene* _scene, const uint64_t _uniqueID );
		~Gameobject();

		void OnGui();

		// Get/add/delete components
		template<typename ComponentType> ComponentType* AddComponent();
		template< typename _componentType >	_componentType* AddEcsComponent() const;
		template<typename ComponentType> ComponentType* GetComponent();
		template< typename _componentType >	_componentType* GetEcsComponent() const;
		template<typename ComponentType> std::vector<ComponentType*> GetComponents();
		template< typename _componentType > void					 RemoveEcsComponent();

		bool							RemoveComponent( const Component* _component );
		Component* AddComponent( const uint32_t _componentID );
		Component* GetComponent( const uint32_t _componentID );
		const std::vector<Component*>& GetComponents() const { return m_components; }

		// Getters
		std::string		GetName() const { return m_name; }
		void			SetName( const std::string _newName ) { m_name = _newName; }
		inline Scene& GetScene() const { return *m_scene; }
		const AABB& GetAABB() const;
		Transform& GetTransform() const { return *m_transform; }
		ecsHandle		GetEcsHandle() { return m_ecsHandleEntity; }
		uint64_t		GetUniqueID() const { return m_uniqueID; }
		bool 			SetUniqueID( const uint64_t _id );

		// Gameobject scene tree parenting
		Gameobject* GetParent() const { return m_parent; }
		const std::vector<Gameobject*>& GetChilds() const { return m_childs; }
		bool IsAncestorOf( const Gameobject* _node ) const;
		void RemoveChild( const Gameobject* _child );
		bool HasChild( const Gameobject* _child );
		void AddChild( Gameobject* _child );
		void SetParent( Gameobject* _parent );
		void InsertBelow( Gameobject* _brother );

		// ISerializable
		bool Save( Json& _json ) const override;
		bool Load( const Json& _json ) override;
		void CopyDataFrom( Json& _json );

		// Flags
		uint32_t	GetFlags() const { return m_flags->flags; }
		void		SetFlags( const uint32_t _flags ) { m_flags->flags = _flags; }
		uint32_t	GetEditorFlags() const { return m_editorFlags->flags; }
		void		SetEditorFlags( const uint32_t _flags ) { m_editorFlags->flags = _flags; }

	private:
		std::string				 m_name;
		uint64_t				 m_uniqueID;
		std::vector<Gameobject*> m_childs;
		Gameobject* m_parent;
		Transform* m_transform = nullptr;
		std::vector<Component*>  m_components;
		Scene* const			 m_scene = nullptr;
		ecsHandle				 m_ecsHandleEntity = ecsNullHandle;

		ecsFlags* const m_flags = nullptr;
		ecsEditorFlags* const m_editorFlags = nullptr;
		ecsAABB* const m_aabb = nullptr;

		void		AddComponent( Component* _component );

	};


	//================================================================================================================================
	// Creates an instance of ComponentType, adds it to the gameobject and returns a pointer
	//================================================================================================================================
	template<typename ComponentType>
	ComponentType* Gameobject::AddComponent()
	{
		// Checks if ComponentType derivates from Component
		static_assert( ( std::is_base_of<Component, ComponentType>::value ) );

		ComponentType* component = new ComponentType();
		if ( GetComponent< ComponentType >() != nullptr )
		{
			Debug::Get() << Debug::Severity::warning << "Trying to add " << ComponentType::s_name << " twice on gameobject " << m_name << Debug::Endl();
			delete( static_cast< Component* >( component ) );
			return nullptr;
		}

		AddComponent( component );

		return component;
	}

	//================================================================================================================================
	// Returns a pointer on the first instance of ComponentType in the gameobject, nullptr if none exists
	//================================================================================================================================
	template<typename ComponentType>
	ComponentType* Gameobject::GetComponent()
	{
		for ( int componentIndex = 0; componentIndex < m_components.size(); componentIndex++ )
		{
			Component* component = m_components[ componentIndex ];

			if ( component->IsType<ComponentType>() )
			{
				return static_cast< ComponentType* >( component );
			}
		}
		return nullptr;
	}

	//================================================================================================================================
	// Gets components of a specific type
	//================================================================================================================================
	template<typename ComponentType>
	std::vector<ComponentType*> Gameobject::GetComponents()
	{
		return static_cast< ComponentType* >( GetComponent( ComponentType::s_typeID ) );
	}

	//================================================================================================================================
	// Returns a component associated with the ecs entityof the gameobject
	//================================================================================================================================
	template< typename _componentType >
	_componentType* Gameobject::GetEcsComponent() const
	{
		static_assert( IsComponent< _componentType>::value );
		return m_scene->GetEcsManager().FindComponentFromHandle<_componentType>( m_ecsHandleEntity );;
	}

	//================================================================================================================================
	// Adds a component to the ecs manager using the ecs entity of the gameobject
	//================================================================================================================================
	template< typename _componentType >	_componentType* Gameobject::AddEcsComponent() const
	{
		static_assert( IsComponent< _componentType>::value );
		EcsManager& ecsManager = m_scene->GetEcsManager();
		ecsEntity entity;
		if ( ecsManager.FindEntity( m_ecsHandleEntity, entity ) )
		{
			return &ecsManager.AddComponent<_componentType>( entity );
		}
		return nullptr;
	}

	//================================================================================================================================
	// Removes a component from the ecs manager using the ecs entity of the gameobject
	//================================================================================================================================
	template< typename _componentType > void Gameobject::RemoveEcsComponent()
	{
		static_assert( IsComponent< _componentType>::value );
		EcsManager& ecsManager = m_scene->GetEcsManager();
		ecsEntity entity;
		if ( ecsManager.FindEntity( m_ecsHandleEntity, entity ) )
		{
			_componentType* component = ecsManager.FindComponentFromEntity<_componentType>( entity );
			component->Clear();
			ecsManager.RemoveComponent<_componentType>( entity );
		}
	}
}