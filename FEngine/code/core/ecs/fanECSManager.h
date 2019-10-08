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

		CEntity CreateEntity();
		void	DeleteEntity( const CEntity  _entity);
		template< typename _componentType > void AddComponent( const CEntity _entity );
		template< typename _tagType >		void AddTag( const CEntity _entity );

		void	Refresh();

		void OnGui();

	private:

		ComponentsTuple< Components > m_components;
		std::vector<EntityData>		  m_entitiesData;

		CEntity m_firstDeadEntity = 0;
		CEntity m_activeEntitiesCount = 0;

		
		void	SortEntities();
		void	RemoveDeadEntities();
 	};

	//================================================================================================================================
	//================================================================================================================================
	template< typename _componentType >
	void EcsManager::AddComponent( const CEntity _entity ) {
		static_assert( IsComponent< _componentType>::value );
		assert( _entity  < m_entitiesData .size() );

		std::vector<_componentType> & components =  m_components.Get< _componentType >();
		EntityData&					  entityData = m_entitiesData[_entity];

		if ( components.size() + 1 >= components.capacity() ) {
			components.reserve( 2 * components.size() );
			Debug::Log( "realloc components" );
		}
		components.push_back( _componentType() );
		entityData.components[ IndexOfComponent<CTranform>::value ] =  static_cast<uint32_t>( components .size() - 1 );
		entityData.bitset[ IndexOfComponent<CTranform>::value ] = 1;
	}
	//================================================================================================================================
	//================================================================================================================================
	template< typename _tagType > void EcsManager::AddTag( const CEntity _entity ) {
		static_assert( IsTag< _tagType>::value );
		m_entitiesData[_entity].bitset[ IndexOfTag<_tagType>::value ] = 1;
	}
}