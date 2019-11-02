#include "fanGlobalIncludes.h"
#include "ecs/fanECSManager.h"
#include "core/time/fanTime.h"
#include "core/time/fanProfiler.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	EcsManager::EcsManager() {
		m_entityToHandles.reserve( 512 );
		m_entitiesKeys.reserve(1024);
	}

	//================================================================================================================================
	//================================================================================================================================
	ecsEntity EcsManager::CreateEntity() {
		if ( m_entitiesKeys.size() == m_entitiesKeys.capacity() ) {
			m_entitiesKeys.reserve( 2 * m_entitiesKeys.size() );
			Debug::Log("realloc entities");
		}
		ecsEntity entity = static_cast<ecsEntity>( m_entitiesKeys.size());
		m_entitiesKeys.push_back( ecsComponentsKey() );
		return static_cast<ecsEntity> ( entity );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EcsManager::DeleteEntity( const ecsEntity  _entity ) {
		m_entitiesKeys[_entity].Kill();
	}

	//================================================================================================================================
	//================================================================================================================================
	ecsHandle EcsManager::CreateHandle( const ecsEntity  _referencedEntity ) {
		ecsHandle handle = m_nextHandle ++;
		m_entityToHandles[_referencedEntity] = handle;
		m_handlesToEntity[handle] = _referencedEntity;
		return handle;		
	}

	//================================================================================================================================
	//================================================================================================================================
	bool EcsManager::FindEntity( const ecsHandle  _handle, ecsEntity& _outEntity ) {
		std::unordered_map< ecsHandle, ecsEntity >::iterator it = m_handlesToEntity.find( _handle );
		if ( it != m_handlesToEntity.end() ) {
			_outEntity = it->second;
			return true;
		}
		return false;
	}

	//================================================================================================================================
	// Called at the end of the frame
	// Cleans, reorders and refreshes the entities, components & handles
	//================================================================================================================================
	void EcsManager::Refresh() {
		SCOPED_PROFILE( ecs_refresh )

		RemoveDeadComponentsAndTags();
		SortEntities();
		RemoveDeadEntities();		
	}

	//================================================================================================================================
	// Helper used to pass the right arguments to systems
	//================================================================================================================================
	template< typename _type, typename _system > struct RunSystem;
	template< template < typename...> typename _components, typename... _types, typename _system  >
	struct RunSystem<_components<_types...>, _system > {
		static void Run( const float _delta, const size_t _count, std::vector<ecsComponentsKey>& _entitiesData, ecsComponentsTuple< ecsComponents >& _tuple ) {
			_system::Run( _delta, _count, _entitiesData,  _tuple.Get<_types>()... );
		}
	};

	//================================================================================================================================
	// Runs the systems before the physics update
	//================================================================================================================================
	#define RUN_SYSTEM( _system ) RunSystem< _system::signature::componentsTypes, _system >::Run( _delta, m_activeEntitiesCount, m_entitiesKeys, m_components );
	void EcsManager::Update( const float _delta, const btVector3& _cameraPosition ) {
		SCOPED_PROFILE( ecs_update )

		ecsParticleSystem::s_cameraPosition = _cameraPosition;
		RUN_SYSTEM(ecsParticleSystem);
		RUN_SYSTEM(ecsPlanetsSystem);
		RUN_SYSTEM(ecsSynchRbToTransSystem);
	}

	//================================================================================================================================
	// Runs the systems after the physics update
	//================================================================================================================================
	void EcsManager::LateUpdate( const float _delta ) {
		SCOPED_PROFILE( ecs_lateUpdate )

		RUN_SYSTEM( ecsSynchTransToRbSystem);
		RUN_SYSTEM( ecsUpdateAABBFromRigidbody );
		RUN_SYSTEM( ecsUpdateAABBFromHull);
		RUN_SYSTEM( ecsUpdateAABBFromTransform);
	}

	//================================================================================================================================
	// Helper for the RecycleComponent method
	// iterates recursively on static components indices, finds the on that matches the runtime index _id 
	// and deletes it
	//================================================================================================================================
	class RecycleHelper {
	private:
		// General case
		template< size_t _index >
		static void RecycleImpl( ecsComponentsTuple< ecsComponents >& _components, const size_t _id, const ecsComponentIndex& _componentIndex ) {
			if ( _index == _id ) {
				_components.Get<_index>().Delete( _componentIndex );
			} else {
				RecycleImpl< _index - 1>( _components, _id, _componentIndex );
			}
		}

		// Specialization 
		template< >
		static void RecycleImpl<0>( ecsComponentsTuple< ecsComponents >& _components, const size_t _id, const ecsComponentIndex& _componentIndex ) {
			if ( _id == 0 ) {
				_components.Get<0>().Delete( _componentIndex );
			} else {
				assert( false ); // Out of range
			}
		}
	public:
		static void Recycle( ecsComponentsTuple< ecsComponents >& _components, const size_t _id, const ecsComponentIndex& _componentIndex ) {
			RecycleImpl< ecsComponents::count - 1 >( _components, _id, _componentIndex );
		}
	};	   

	//================================================================================================================================
	// put _componentIndex in the recycleList of component _componentID
	//================================================================================================================================
	void EcsManager::RecycleComponent( const uint32_t _componentID, const ecsComponentIndex& _componentIndex ) {
		assert( _componentID  < ecsComponents::count );
		RecycleHelper::Recycle( m_components, _componentID, _componentIndex );
	}

	//================================================================================================================================
	// Removes the dead entities at the end of the entity vector
	//================================================================================================================================
	void EcsManager::RemoveDeadEntities() {
		if ( m_entitiesKeys.empty() ) {
			return;
		}

		int64_t reverseIndex = m_entitiesKeys.size() - 1;
		while ( reverseIndex >= 0 ) {
			ecsComponentsKey& key = m_entitiesKeys[reverseIndex];
			if ( key.IsAlive() ) {
				break; // We processed all dead entities
			}

			// Remove corresponding handle
			std::unordered_map< ecsEntity, ecsHandle >::iterator it = m_entityToHandles.find( (ecsEntity)reverseIndex );
			if ( it != m_entityToHandles.end() ) {
				m_handlesToEntity.erase( it->second );
				m_entityToHandles.erase( (ecsEntity)reverseIndex );
			}

			// Remove the component
			for (int componentID = 0; componentID < ecsComponents::count ; componentID++) {
				if( key.HasComponent(componentID) ) {
					ecsComponentIndex ecsIndex = key.RemoveComponent( componentID );
					RecycleComponent( componentID, ecsIndex );
				}
			}
			m_entitiesKeys.pop_back();
			--reverseIndex;
		}
		m_activeEntitiesCount = static_cast<ecsEntity>( m_entitiesKeys.size() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void  EcsManager::RemoveDeadComponentsAndTags() {
		// Remove components
		for ( std::pair< ecsEntity, uint32_t> & pair : m_removedComponents ) {
			const ecsEntity entity = pair.first;
			const  uint32_t componentID = pair.second;
			assert( m_entitiesKeys[entity].HasComponent(componentID) );			
			ecsComponentsKey& key = m_entitiesKeys[entity];
			ecsComponentIndex ecsIndex = key.RemoveComponent( componentID );
			RecycleComponent( componentID, ecsIndex );
			
		} m_removedComponents.clear();

		// Removes tags
		for ( std::pair< ecsEntity, uint32_t> & pair : m_removedTags ) {
			const ecsEntity entity = pair.first;
			const  uint32_t tagIndex = pair.second;

			assert( m_entitiesKeys[entity].HasTag(tagIndex) );
			m_entitiesKeys[entity].SetTag(tagIndex,  false );			

		} m_removedTags.clear();
	}

	//================================================================================================================================
	// Change the entity referenced by a handle
	//================================================================================================================================
	void EcsManager::SwapHandlesEntities( const ecsEntity _entity1, const ecsEntity _entity2 ) {
		std::unordered_map< ecsEntity, ecsHandle >::iterator it1 = m_entityToHandles.find( _entity1 );
		std::unordered_map< ecsEntity, ecsHandle >::iterator it2 = m_entityToHandles.find( _entity2 );

		bool hasHandle1 = it1 != m_entityToHandles.end();
		bool hasHandle2 = it2 != m_entityToHandles.end();
		ecsHandle handle1= 0, handle2 = 0;
		if ( hasHandle1 ) {
			handle1 = it1->second;
			m_entityToHandles.erase(it1);
			m_handlesToEntity.erase(handle1);
		}
		if ( hasHandle2 ) {
			handle2 = it2->second;
			m_entityToHandles.erase( it2 );
			m_handlesToEntity.erase( handle2 );
		}
		if ( hasHandle1 ) {
			m_entityToHandles[_entity2] = handle1;
			m_handlesToEntity[handle1] = _entity2;
		}
		if ( hasHandle2 ) {
			m_entityToHandles[_entity1] = handle2;
			m_handlesToEntity[handle2] = _entity1;
		}

	}

	//================================================================================================================================
	// Place the dead entities at the end of the vector
	//================================================================================================================================
	void EcsManager::SortEntities() {
		const int64_t size = static_cast<int64_t>(m_entitiesKeys.size());
		int64_t forwardIndex = 0;
		int64_t reverseIndex = m_entitiesKeys.size() - 1;

		while( true ) {
			while ( forwardIndex < size - 1 && m_entitiesKeys[forwardIndex].IsAlive()  ) { ++forwardIndex; } // Finds a dead entity
			if( forwardIndex == size - 1 ) break;

			while ( reverseIndex > 0 && m_entitiesKeys[reverseIndex].IsDead() )   { --reverseIndex; } // Finds a live entity
			if ( reverseIndex == 0 ) break;

			if ( forwardIndex > reverseIndex  ) break;

			// updates the indices of the corresponding handle if necessary
			SwapHandlesEntities((ecsEntity) reverseIndex ,(ecsEntity) forwardIndex );

			std::swap( m_entitiesKeys[reverseIndex] , m_entitiesKeys[forwardIndex] );	
			++forwardIndex; --reverseIndex;
		}
	}
}