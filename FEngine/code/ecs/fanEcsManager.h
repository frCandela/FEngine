#pragma once

#include "ecs/fanECSSystems.h"

namespace fan {
	//================================================================================================================================
	// /!\ When adding components, always call the corresponding onRealloc callback to update the binded data  /!\
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

		template< typename _componentType > _componentType& AddComponent( const ecsEntity _entity );
		template< typename _componentType > void			RemoveComponent( const ecsEntity _entity );
		template< typename _tagType >		void			AddTag( const ecsEntity _entity );
		template< typename _tagType >		void			RemoveTag( const ecsEntity _entity );

		void Update( const float _delta, const btVector3& _cameraPosition );
		void LateUpdate( const float _delta );
		void Refresh();

		// Getters 
		const std::vector<ecsComponentsKey>&				GetEntitiesData() const { return m_entitiesKeys;	}
		const ecsComponentsTuple< ecsComponents >&			GetComponents() const	{ return m_components;		}
		const std::unordered_map< ecsHandle, ecsEntity > &	GetHandles() const		{ return m_handlesToEntity; }
		
	private:
		ecsComponentsTuple< ecsComponents >				m_components;
		std::vector<ecsComponentsKey>					m_entitiesKeys;
		std::unordered_map< ecsHandle, ecsEntity >		m_handlesToEntity;
		std::unordered_map< ecsEntity, ecsHandle >		m_entityToHandles;
		std::vector< std::pair< ecsEntity, uint32_t > > m_removedComponents;
		std::vector< std::pair< ecsEntity, uint32_t > > m_removedTags;

		ecsHandle m_nextHandle;
		ecsEntity m_firstDeadEntity = 0;
		ecsEntity m_activeEntitiesCount = 0;
		
		void	SwapHandlesEntities( const ecsEntity _entity1, const ecsEntity _entity2 );
		void	RecycleComponent( const uint32_t _componentID, const ecsComponentIndex& _componentIndex );
		void	SortEntities();
		void	RemoveDeadComponentsAndTags();
		void	RemoveDeadEntities();
 	};

	//================================================================================================================================
	//================================================================================================================================
	template< typename _componentType >
	_componentType& EcsManager::AddComponent( const ecsEntity _entity ) {
		static_assert( IsComponent< _componentType>::value );

		
		const uint32_t componentID = (uint32_t) IndexOfComponent<_componentType>::value;

		assert( _entity < m_entitiesKeys.size() );
		ecsComponentsKey & entityKey = m_entitiesKeys[_entity];

		assert( entityKey.bitset[componentID] == 0 ); // entity already has _componentType

		_componentType & component =  m_components.Alloc<_componentType>( entityKey.index[componentID] );

		// Init the component
		entityKey.bitset    [ componentID ] = 1;

		return component;
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
		m_entitiesKeys[_entity].bitset[ IndexOfTag<_tagType>::value ] = 1;
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
			return FindComponentFromEntity< _componentType >(entity);
		}
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	template< typename _componentType > _componentType* EcsManager::FindComponentFromEntity( const ecsEntity  _entity ) {
		static_assert( IsComponent< _componentType >::value );
		return &m_components.Get< _componentType >()[ m_entitiesKeys[_entity] ];
	}
}