#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "fanSystem.hpp"
#include "ecs/fanEntity.hpp"
#include "core/imgui/fanImguiIcons.hpp"
#include "fanComponentsCollection.hpp"
#include "ecs/fanTag.hpp"

namespace fan {
	struct SingletonComponent;

	//==============================================================================================================================================================
	// Contains the entities, components, singleton components, type information
	// and various utilities for processing them
	//==============================================================================================================================================================
	class EcsWorld
	{
	public:
		EcsWorld( void ( *initializeTypes )( EcsWorld& ) );
		~EcsWorld();

		template< typename _componentType >	bool			HasComponent( const EntityID _entityID );
		template< typename _componentType >	_componentType&	GetComponent( const EntityID _entityID );
		template< typename _componentType >	_componentType&	AddComponent( const EntityID _entityID );
		template< typename _componentType >	void			RemoveComponent( const EntityID _entityID );
		template< typename _tagType >void					AddTag( const EntityID _entityID );
		template< typename _tagType >void					RemoveTag( const EntityID _entityID );
		template< typename _componentType > bool			IsType( const Component& _component );
		template< typename _componentType >	_componentType&	GetSingletonComponent();
		template< typename _tagOrComponentType > Signature	GetSignature() const;

		Component&		  AddComponent( const EntityID _entityID, const ComponentIndex _index );
		Component&		  GetComponent( const EntityID _entityID, const ComponentIndex _index );
		void				  RemoveComponent( const EntityID _entityID, const ComponentIndex _index );
		bool				  HasComponent( const EntityID _entityID, ComponentIndex _index );
		const ComponentInfo&  GetComponentInfo( const ComponentIndex _index ) const { return  m_componentInfo.at( _index );	}
		EntityID			  CreateEntity();
		void				  KillEntity( const EntityID _entityID );
		EntityHandle		  CreateHandle( const EntityID _entityID );
		EntityID			  GetEntityID( const EntityHandle _handle );
		ComponentIndex		  GetDynamicIndex( const uint32_t _staticIndex ) { return m_typeIndices[_staticIndex]; }
		void				  SortEntities();
		void				  RemoveDeadEntities();
		uint32_t			  GetComponentCount( const EntityID _entityID ) { return m_entities[_entityID].componentCount;  }
		Component&		  GetComponentAt( const EntityID _entityID, int _componentIndex );

		std::vector<EntityID> Match( const Signature _signature );
		std::vector<EntityID> MatchSubset( const Signature _signature, const std::vector<EntityID>& _subset );

		// add types
		template< typename _componentType >	void AddComponentType();
		template< typename _tagType >		void AddTagType();
		template< typename _componentType >	void AddSingletonComponentType();

		// const accessors
		const std::unordered_map< uint32_t, ComponentIndex >&   GetDynamicIndices() const { return m_typeIndices; }
		const std::unordered_map< EntityHandle, EntityID >&		GetHandles() const { return m_handles; }
		const std::vector< ComponentsCollection >&				GetComponentCollections() const { return m_components; }
		const std::vector< Entity >&							GetEntities() const				{ return m_entities; }
		void	GetVectorComponentInfo( std::vector< const ComponentInfo*>& _outVector ) const;
		size_t	GetNumSingletonComponents() const { return m_singletonComponents.size(); }
		size_t	GetNumEntities() const { return m_entities.size();  }

	private:
		std::unordered_map< uint32_t, ComponentIndex >		m_typeIndices;
		std::unordered_map< EntityHandle, EntityID >		m_handles;
		std::unordered_map< uint32_t, SingletonComponent* >	m_singletonComponents;
		std::unordered_map< ComponentIndex, ComponentInfo >	m_componentInfo;	

		std::vector< Entity >				m_entities;
		std::vector< ComponentsCollection > m_components;

		EntityHandle	m_nextHandle = 1; // 0 is a null handle
		ComponentIndex	m_nextTypeIndex = 0;
		ComponentIndex	m_nextTagIndex = signatureLength - 2;

		Entity& GetEntity( const EntityID _id );
		bool	EntityMatchSignature( EntityID _entityID, const Signature& _signature );
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _componentType >
	_componentType& EcsWorld::AddComponent( const EntityID _entityID )
	{
		static_assert( std::is_base_of< Component, _componentType>::value );
		const ComponentIndex index = m_typeIndices[_componentType::s_typeInfo];
		Component& component = AddComponent( _entityID, index );
		return static_cast< _componentType& >( component );
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _componentType >	
	_componentType& EcsWorld::GetComponent( const EntityID _entityID )
	{
		static_assert( std::is_base_of< Component, _componentType>::value );			
		const ComponentIndex index = m_typeIndices[_componentType::s_typeInfo];
		return static_cast<_componentType&> ( GetComponent( _entityID, index ) );
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _componentType >	bool EcsWorld::HasComponent( const EntityID _entityID )
	{
		static_assert( std::is_base_of< Component, _componentType>::value );
		const ComponentIndex index = m_typeIndices[_componentType::s_typeInfo];
		return m_entities[_entityID].HasComponent( index );
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _componentType >	void EcsWorld::RemoveComponent( const EntityID _entityID )
	{
		static_assert( std::is_base_of< Component, _componentType>::value );
		const ComponentIndex index = m_typeIndices[_componentType::s_typeInfo];
		RemoveComponent( _entityID, index );
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _tagType > void EcsWorld::AddTag( const EntityID _entityID )
	{
		static_assert( std::is_base_of< Tag, _tagType>::value );
		Entity& entity = GetEntity( _entityID );
		const ComponentIndex index = m_typeIndices[_tagType::s_typeInfo];
		entity.signature[index] = 1;
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _tagType >void EcsWorld::RemoveTag( const EntityID _entityID )
	{
		static_assert( std::is_base_of< Tag, _tagType>::value );
		Entity& entity = GetEntity( _entityID );
		const ComponentIndex index = m_typeIndices[_tagType::s_typeInfo];
		entity.signature[index] = 0;
	}

	//==============================================================================================================================================================
	// returns true if the given component has the same type as _componentType
	//==============================================================================================================================================================
	template< typename _componentType > bool EcsWorld::IsType( const Component& _component )
	{
		static_assert( std::is_base_of< Component, _componentType>::value );
		return _component.GetIndex() == GetDynamicIndex( _componentType::s_typeInfo );
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _componentType > _componentType& EcsWorld::GetSingletonComponent()
	{
		static_assert( std::is_base_of< SingletonComponent, _componentType>::value );
		return  *static_cast<_componentType*>( m_singletonComponents[_componentType::s_typeInfo] );
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _tagOrComponentType > Signature EcsWorld::GetSignature() const
	{
		static_assert( std::is_base_of< Tag, _tagOrComponentType>::value || std::is_base_of< Component, _tagOrComponentType>::value );
		return Signature( 1 ) << m_typeIndices.at( _tagOrComponentType::s_typeInfo );
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _componentType >	void EcsWorld::AddComponentType()
	{
		static_assert( std::is_base_of< Component, _componentType>::value );
		assert( m_nextTagIndex >= m_nextTypeIndex );

		m_components.push_back( ComponentsCollection() );
		ComponentsCollection& collection = m_components[m_components.size() - 1];
		collection.Init<_componentType>( _componentType::s_typeName );
		const ComponentIndex index = m_nextTypeIndex++;
		m_typeIndices[_componentType::s_typeInfo] = index;

		// Registers component info
		ComponentInfo info;
		_componentType::SetInfo( info );
		assert( info.init != nullptr );
		info.name = _componentType::s_typeName;
		info.instanciate = &_componentType::Instanciate;
		info.index = index;	
		info.staticIndex = _componentType::s_typeInfo;
		m_componentInfo[index] = info;		
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _tagType > void EcsWorld::AddTagType()
	{
		static_assert( std::is_base_of< Tag, _tagType>::value );
		assert( m_nextTagIndex >= m_nextTypeIndex );
		m_typeIndices[_tagType::s_typeInfo] = m_nextTagIndex--;		
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _componentType >	void EcsWorld::AddSingletonComponentType()
	{
		static_assert( std::is_base_of< SingletonComponent, _componentType>::value );
		assert( m_singletonComponents.find( _componentType::s_typeInfo ) == m_singletonComponents.end() );
		_componentType * component = new _componentType();
		m_singletonComponents[_componentType::s_typeInfo] = component;
	}
}