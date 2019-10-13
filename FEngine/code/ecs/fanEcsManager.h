#pragma once

#include "ecs/fanECSSystems.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class EcsManager {
	public:
		EcsManager();	

		ecsEntity	CreateEntity();
		void		DeleteEntity( const ecsEntity  _entity);
		ecsHandle	CreateHandle( const ecsEntity  _referencedEntity );

		bool												FindEntity( const ecsHandle  _handle, ecsEntity& _outEntity );
		template< typename _componentType > _componentType* FindComponentFromHandle( const ecsHandle  _handle );
		template< typename _componentType > _componentType* FindComponentFromEntity( const ecsEntity  _entity );

		template< typename _componentType > uint32_t AddComponent( const ecsEntity _entity );
		template< typename _componentType > void	 RemoveComponent( const ecsEntity _entity );
		template< typename _tagType >		void	 AddTag( const ecsEntity _entity );
		template< typename _tagType >		void	 RemoveTag( const ecsEntity _entity );

		void Update( float _delta );
		void LateUpdate( float _delta );
		void Refresh();

		// Getters 
		const std::vector<ecsEntityData>&					GetEntitiesData() const { return m_entitiesData;	}
		const ecsComponentsTuple< ecsComponents >&			GetComponents() const	{ return m_components;		}
		const std::unordered_map< ecsHandle, ecsEntity > &	GetHandles() const		{ return m_handlesToEntity; }
		
	private:
		ecsComponentsTuple< ecsComponents >			m_components;
		std::vector<ecsEntityData>					m_entitiesData;
		std::unordered_map< ecsHandle, ecsEntity >  m_handlesToEntity;
		std::unordered_map< ecsEntity, ecsHandle >  m_entityToHandles;
		std::vector< std::pair< ecsEntity, uint32_t > > m_removedComponents;
		std::vector< std::pair< ecsEntity, uint32_t > > m_removedTags;

		ecsHandle m_nextHandle;
		ecsEntity m_firstDeadEntity = 0;
		ecsEntity m_activeEntitiesCount = 0;
		
		void	SwapHandlesEntities( const ecsEntity _entity1, const ecsEntity _entity2 );
		void	RecycleComponent( const uint32_t _componentID, const uint32_t _componentIndex );
		void	SortEntities();
		void	RemoveDeadComponentsAndTags();
		void	RemoveDeadEntities();
 	};



	//================================================================================================================================
	//================================================================================================================================
	template< typename _componentType >
	uint32_t EcsManager::AddComponent( const ecsEntity _entity ) {
		static_assert( IsComponent< _componentType>::value );
		assert( _entity  < m_entitiesData.size() );
		assert( m_entitiesData[_entity].bitset[IndexOfComponent<_componentType>::value] == 0 ); // entity already has _componentType

		// Get references
		auto &							componentData =  m_components.Get< _componentType >();
		std::vector<_componentType>&	vector = componentData.vector;
		std::vector<uint32_t>&			recycleList = componentData.recycleList;
		ecsEntityData &					entityData = m_entitiesData[_entity];

		// Get new component index
		uint32_t componentIndex;		
		if ( recycleList.empty() ) { // Create new index
			if ( vector.size() + 1 >= vector.capacity() ) {
				vector.reserve( 2 * vector.size() );
				Debug::Log( "realloc components" );
			}
			componentIndex = static_cast<uint32_t>( vector.size() );
			vector.push_back( _componentType() );
		} else { // Reuse old index
			componentIndex = recycleList[ recycleList.size() - 1];
			recycleList.pop_back();
		}

		// Init the component
		entityData.components[ IndexOfComponent<_componentType>::value ] =  static_cast<uint32_t>( componentIndex );
		entityData.bitset[ IndexOfComponent<_componentType>::value ] = 1;

		return componentIndex;
	}
	
	//================================================================================================================================
	//================================================================================================================================
	template< typename _componentType > 
	void  EcsManager::RemoveComponent( const ecsEntity _entity) {
		static_assert( IsComponent< _componentType>::value );
		m_removedComponents.push_back( std::make_pair( _entity, (uint32_t) IndexOfComponent< _componentType  >::value ));
	}

	//================================================================================================================================
	//================================================================================================================================
	template< typename _tagType > void EcsManager::AddTag( const ecsEntity _entity ) {
		static_assert( IsTag< _tagType>::value );
		m_entitiesData[_entity].bitset[ IndexOfTag<_tagType>::value ] = 1;
	}

	//================================================================================================================================
	//================================================================================================================================
	template< typename _tagType >
	void EcsManager::RemoveTag( const ecsEntity _entity ) {
		static_assert( IsTag< _tagType>::value );
		m_removedTags.push_back( std::make_pair( _entity, (uint32_t)IndexOfTag< _tagType  >::value ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	template< typename _componentType > 
	_componentType* EcsManager::FindComponentFromHandle( const ecsHandle  _handle ) {
		static_assert( IsComponent< _componentType >::value );
		ecsEntity entity;
		if ( FindEntity( _handle, entity ) ) {
			return &m_components.Get< _componentType >().vector[m_entitiesData[entity].components[IndexOfComponent<_componentType>::value]];			
		}
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	template< typename _componentType > _componentType* EcsManager::FindComponentFromEntity( const ecsEntity  _entity ) {
		static_assert( IsComponent< _componentType >::value );
		return &m_components.Get< _componentType >().vector[m_entitiesData[_entity].components[IndexOfComponent<_componentType>::value]];
	}
}