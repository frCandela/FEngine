#pragma once

#include "core/ecs/fanEcsConfig.h"
#include "core/ecs/fanComponentsTuple.h"
#include "core/ecs/EntityData.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class EcsManager {
	public:
		EcsManager();	

		ecsEntity	CreateEntity();
		void		DeleteEntity( const ecsEntity  _entity);
		ecsHandle	CreateHandle( const ecsEntity  _referencedEntity );
		bool		FindEntity( const ecsHandle  _handle, ecsEntity& _outEntity );

		template< typename _componentType > void AddComponent( const ecsEntity _entity );
		template< typename _tagType >		void AddTag( const ecsEntity _entity );

		void	Refresh();

		void OnGui();

	private:
		ecsComponentsTuple< ecsComponents >			m_components;
		std::vector<ecsEntityData>					m_entitiesData;
		std::unordered_map< ecsHandle, ecsEntity >  m_handlesToEntity;
		std::unordered_map< ecsEntity, ecsHandle >  m_entityToHandles;

		ecsHandle m_nextHandle;
		ecsEntity m_firstDeadEntity = 0;
		ecsEntity m_activeEntitiesCount = 0;
		
		void	SwapHandlesEntities( const ecsEntity _entity1, const ecsEntity _entity2 );
		void	SortEntities();
		void	RemoveDeadEntities();
 	};

	//================================================================================================================================
	//================================================================================================================================
	template< typename _componentType >
	void EcsManager::AddComponent( const ecsEntity _entity ) {
		static_assert( IsComponent< _componentType>::value );
		assert( _entity  < m_entitiesData .size() );

		auto &		componentData =  m_components.Get< _componentType >();
		std::vector<_componentType>&	vector = componentData.vector;
		std::vector<uint32_t>&			recycleList = componentData.recycleList;
		ecsEntityData &					entityData = m_entitiesData[_entity];

		uint32_t componentIndex;
		if ( recycleList.empty() ) {
			if ( vector.size() + 1 >= vector.capacity() ) {
				vector.reserve( 2 * vector.size() );
				Debug::Log( "realloc components" );
			}
			componentIndex = static_cast<uint32_t>( vector.size() );
			vector.push_back( _componentType() );
		} else {
			componentIndex = recycleList[ recycleList.size() - 1];
			recycleList.pop_back();
		}
		entityData.components[ IndexOfComponent<_componentType>::value ] =  static_cast<uint32_t>( componentIndex );
		entityData.bitset[ IndexOfComponent<_componentType>::value ] = 1;
	}
	//================================================================================================================================
	//================================================================================================================================
	template< typename _tagType > void EcsManager::AddTag( const ecsEntity _entity ) {
		static_assert( IsTag< _tagType>::value );
		m_entitiesData[_entity].bitset[ IndexOfTag<_tagType>::value ] = 1;
	}
}