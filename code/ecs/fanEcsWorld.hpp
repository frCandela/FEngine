#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "fanSystem.hpp"
#include "ecs/fanTag.hpp"
#include "ecs/fanEntity.hpp"
#include "ecs/fanSingletonComponent.hpp"
#include "fanComponentsCollection.hpp"


namespace fan {
	struct SingletonComponent;

	//==============================================================================================================================================================
	// Contains the entities, components, singleton components and runtime type information
	// component, tags and singleton are added dynamically when creating the ecs
	// they all have a static id based on their name and an ecs local dynamic id. These ids are unique
	// Entity ids will change from one frame to the other, use a EntityHandle to keep track of an entity
	//==============================================================================================================================================================
	class EcsWorld
	{
	public:
		EcsWorld();
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

		Component&			 AddComponent( const EntityID _entityID, const ComponentIndex _index );
		Component&			 GetComponent( const EntityID _entityID, const ComponentIndex _index );
		SingletonComponent&	 GetSingletonComponent( const uint32_t _staticIndex );
		void				 RemoveComponent( const EntityID _entityID, const ComponentIndex _index );
		bool				 HasComponent( const EntityID _entityID, ComponentIndex _index );
		void				 AddTagsFromSignature( const EntityID _entityID, const Signature& _signature );
		Signature			 AliveSignature() const { return ( Signature( 1 ) << ecAliveBit ); }
		EntityID			 CreateEntity();
		void				 KillEntity( const EntityID _entityID );
		void				 Clear();
		EntityHandle		 CreateHandle( const EntityID _entityID );
		EntityID			 GetEntityID( const EntityHandle _handle );
		ComponentIndex		 GetDynamicIndex( const uint32_t _staticIndex ) const { return m_typeIndices.at(_staticIndex); }
		uint32_t			 GetComponentCount( const EntityID _entityID ) { return m_entities[_entityID].componentCount;  }
		Component&			 GetComponentAt( const EntityID _entityID, int _componentIndex );
		const ComponentInfo& GetComponentInfo( const ComponentIndex _index ) const { return  m_componentInfo[_index]; }
		const SingletonComponentInfo& GetSingletonComponentInfo( const uint32_t _staticIndex ) const { return  m_singletonComponentInfo.at( _staticIndex ); }
		const SingletonComponentInfo* SafeGetSingletonComponentInfo( const uint32_t _staticIndex ) const;


		void				 SortEntities();
		void				 RemoveDeadEntities();

		std::vector<EntityID> Match( const Signature _signature );
		std::vector<EntityID> MatchSubset( const Signature _signature, const std::vector<EntityID>& _subset );

		// add types
		template< typename _componentType >	void AddComponentType();
		template< typename _tagType >		void AddTagType();
		template< typename _componentType >	_componentType& AddSingletonComponentType();

		// const accessors
		const std::unordered_map< uint32_t, ComponentIndex >&   GetDynamicIndices() const		{ return m_typeIndices; }
		const std::unordered_map< EntityHandle, EntityID >&		GetHandles() const				{ return m_handles; }
		const std::vector< ComponentsCollection >&				GetComponentCollections() const { return m_components; }
		const std::vector< Entity >&							GetEntities() const				{ return m_entities; }
		const std::vector< ComponentInfo >&						GetVectorComponentInfo() const  { return m_componentInfo;  }
		std::vector< SingletonComponentInfo >					GetVectorSingletonComponentInfo() const;
		size_t	GetNumEntities() const { return m_entities.size(); }

	private:
		std::unordered_map< uint32_t, ComponentIndex >		   m_typeIndices;
		std::unordered_map< EntityHandle, EntityID >		   m_handles;
		std::unordered_map< uint32_t, SingletonComponent* >	   m_singletonComponents;
		std::vector< ComponentInfo >						   m_componentInfo;
		std::unordered_map< uint32_t, SingletonComponentInfo > m_singletonComponentInfo;

		std::vector< Entity >				m_entities;
		std::vector< ComponentsCollection > m_components;

		EntityHandle	m_nextHandle = 1; // 0 is a null handle
		ComponentIndex	m_nextTypeIndex = 0;
		ComponentIndex	m_nextTagIndex = signatureLength - 2;

		Signature m_tagsMask;	// signature of all tag types combined

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
		return  static_cast<_componentType&>( GetSingletonComponent(_componentType::s_typeInfo) );
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _tagOrComponentType > Signature EcsWorld::GetSignature() const
	{
		static_assert( std::is_base_of< Tag, _tagOrComponentType>::value || std::is_base_of< Component, _tagOrComponentType>::value );
		return Signature( 1 ) << m_typeIndices.at( _tagOrComponentType::s_typeInfo );
	}

	//==============================================================================================================================================================
	// creates a component type and adds it to the ecs world
	//==============================================================================================================================================================
	template< typename _componentType >	void EcsWorld::AddComponentType()
	{
		static_assert( std::is_base_of< Component, _componentType>::value );
		assert( m_nextTagIndex >= m_nextTypeIndex );

		// creates collection
		m_components.push_back( ComponentsCollection() );
		ComponentsCollection& collection = m_components[m_components.size() - 1];
		collection.Init<_componentType>( _componentType::s_typeName );
		
		// generate dynamic index
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
		m_componentInfo.push_back( info );		
	}

	//==============================================================================================================================================================
	// creates a tag type and adds it to the ecs world
	//==============================================================================================================================================================
	template< typename _tagType > void EcsWorld::AddTagType()
	{
		static_assert( std::is_base_of< Tag, _tagType>::value );
		assert( m_nextTagIndex >= m_nextTypeIndex );
		const ComponentIndex newTagIndex = m_nextTagIndex--;
		m_typeIndices[_tagType::s_typeInfo] = newTagIndex;

		m_tagsMask[newTagIndex] = 1; 
	}

	//==============================================================================================================================================================
	// creates a singleton component type and adds it to the ecs world
	//==============================================================================================================================================================
	template< typename _componentType >	_componentType& EcsWorld::AddSingletonComponentType()
	{
		static_assert( std::is_base_of< SingletonComponent, _componentType>::value );
		assert( m_singletonComponents.find( _componentType::s_typeInfo ) == m_singletonComponents.end() );
		
		// creates the singleton component
		_componentType * component = new _componentType();
		m_singletonComponents[_componentType::s_typeInfo] = component;

		// Registers component info
		SingletonComponentInfo info;
		_componentType::SetInfo( info );
		assert( info.init != nullptr );
		info.init( *this, *component );
		info.staticIndex = _componentType::s_typeInfo;
		m_singletonComponentInfo[ _componentType::s_typeInfo ] = info ;

		return *component;
	}
}