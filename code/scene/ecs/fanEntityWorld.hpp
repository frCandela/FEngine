#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEntity.hpp"
#include "core/imgui/fanImguiIcons.hpp"

namespace fan {
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct ComponentInfo
	{
		std::string name;
		ImGui::IconType icon = ImGui::IconType::NONE;		
		void ( *onGui )( ecComponent& ) = nullptr;
		void ( *clear )( ecComponent& ) = nullptr;
		void ( *save )( const ecComponent&, Json& ) = nullptr;
		void ( *load )( ecComponent&, const Json& ) = nullptr;
		ecComponent& ( *instanciate)( void* ) = nullptr;
		const char* editorPath = "";
		ComponentIndex index;
		uint32_t staticIndex;
	};

	//==============================================================================================================================================================
	// Contains the entities, components, singleton components, type information
	// and various utilities for processing them
	//==============================================================================================================================================================
	class EntityWorld
	{
	public:
		EntityWorld();

		template< typename _componentType >	_componentType& AddComponent( const EntityID _entityID );
		template< typename _componentType >	void			RemoveComponent( EntityID _entityID );
		template< typename _tagType >void					AddTag( EntityID _entityID );
		template< typename _componentType >	_componentType& GetSingletonComponent();
		template< typename _systemType > void				RunSystem( const float _delta );
		template< typename _tagOrComponentType > Signature	GetSignature() const;

		ecComponent&		  AddComponent( const EntityID _entityID, const ComponentIndex _index );
		bool				  HasComponent( const EntityID _entityID, ComponentIndex _index );
		const ComponentInfo&  GetComponentInfo( const ComponentIndex _index ) const { return  m_componentInfo.at( _index );	}
		EntityID			  CreateEntity();
		void				  KillEntity( EntityID _entityID );
		EntityHandle		  CreateHandle( EntityID _entityID );
		EntityID			  GetEntityID( EntityHandle _handle );
		Entity&				  GetEntity( const EntityID _id );
		void				  SortEntities();
		void				  RemoveDeadEntities();
	//private:

		template< typename _componentType >	void AddComponentType();
		template< typename _tagType >		void AddTagType();
		template< typename _componentType >	void AddSingletonComponentType();

		std::unordered_map< uint32_t, ComponentIndex >		m_typeIndices;
		std::unordered_map< EntityHandle, EntityID >		m_handles;
		std::unordered_map< uint32_t, SingletonComponent* >	m_singletonComponents;		
		std::unordered_map< ComponentIndex, ComponentInfo >	m_componentInfo;

		std::vector< Entity >				m_entities;
		std::vector< ComponentsCollection > m_components;

		EntityHandle	m_nextHandle = 1; // 0 is a null handle
		ComponentIndex	m_nextTypeIndex = 0;
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _componentType >
	_componentType& EntityWorld::AddComponent( const EntityID _entityID )
	{
		static_assert( std::is_base_of< ecComponent, _componentType>::value );
		const ComponentIndex index = m_typeIndices[_componentType::s_typeInfo];
		ecComponent& component = AddComponent( _entityID, index );
		return static_cast< _componentType& >( component );
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _componentType >	void EntityWorld::RemoveComponent( EntityID _entityID )
	{
		Entity& entity = GetEntity( _entityID );
		assert( entity.signature[_componentType::s_typeID] == 1 ); // this entity doesn't have this component
		_componentType& component = entity.GetComponent<_componentType>();
		m_components[_componentType::s_typeID].RemoveComponent( component.chunckIndex, component.index );
		entity.signature[_componentType::s_typeID] = 0;

		for( int componentIndex = 0; componentIndex < entity.componentCount; componentIndex++ )
		{
			if( entity.components[componentIndex]->typeID == _componentType::s_typeID )
			{
				entity.componentCount--;
				entity.components[componentIndex] = entity.components[entity.componentCount]; // swap
				entity.components[entity.componentCount] = nullptr;
				return;
			}
		}
		assert( false ); // component not found
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _tagType > void EntityWorld::AddTag( EntityID _entityID )
	{
		static_assert( std::is_base_of< Tag, _tagType>::value );
		Entity& entity = GetEntity( _entityID );
		const ComponentIndex index = m_typeIndices[_tagType::s_typeInfo];
		entity.signature[index] = 1;
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _componentType > _componentType& EntityWorld::GetSingletonComponent()
	{
		static_assert( std::is_base_of< SingletonComponent, _componentType>::value );
		return  *static_cast<_componentType*>( m_singletonComponents[_componentType::s_typeInfo] );
	}

	//================================================================================================================================
	// Find all entities matching the signature of the system and runs it
	//================================================================================================================================
	template< typename _systemType > void EntityWorld::RunSystem( const float _delta )
	{
		static_assert( std::is_base_of< System, _systemType>::value );
		std::vector<Entity*> matchEntities;

		const Signature systemSignature = _systemType::GetSignature( *this );

		matchEntities.reserve( m_entities.size() );
		for( int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
		{
			Entity& entity = m_entities[entityIndex];
			if( ( entity.signature & systemSignature ) == systemSignature )
			{
				matchEntities.push_back( &entity );
			}
		}
		_systemType::Run( *this, matchEntities, _delta );
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _tagOrComponentType > Signature EntityWorld::GetSignature() const
	{
		static_assert( std::is_base_of< Tag, _tagOrComponentType>::value || std::is_base_of< ecComponent, _tagOrComponentType>::value );
		return Signature( 1 ) << m_typeIndices.at( _tagOrComponentType::s_typeInfo );
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _componentType >	void EntityWorld::AddComponentType()
	{
		static_assert( std::is_base_of< ecComponent, _componentType>::value );
		ComponentsCollection chunck;
		chunck.Init<_componentType>( _componentType::s_typeName );
		m_components.push_back( chunck );
		const ComponentIndex index = m_nextTypeIndex++;
		m_typeIndices[_componentType::s_typeInfo] = index;

		// Registers component info
		ComponentInfo info;
		_componentType::SetInfo( info );
		assert( info.clear != nullptr );
		info.name = _componentType::s_typeName;
		info.instanciate = &_componentType::Instanciate;
		info.index = index;	
		info.staticIndex = _componentType::s_typeInfo;
		m_componentInfo[index] = info;		
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _tagType > void EntityWorld::AddTagType()
	{
		static_assert( std::is_base_of< Tag, _tagType>::value );
		m_typeIndices[_tagType::s_typeInfo] = m_nextTypeIndex++;
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _componentType >	void EntityWorld::AddSingletonComponentType()
	{
		static_assert( std::is_base_of< SingletonComponent, _componentType>::value );
		assert( m_singletonComponents.find( _componentType::s_typeInfo ) == m_singletonComponents.end() );
		m_singletonComponents[_componentType::s_typeInfo] = new _componentType();
	}
}