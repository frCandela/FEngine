#include "fanGlobalIncludes.h"
#include "ecs/fanECSManager.h"
#include "core/fanTime.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	EcsManager::EcsManager() {
		m_entityToHandles.reserve( 512 );
		m_entitiesData.reserve(1024);
		m_components.Get<ecsTranform>().vector.reserve( 512 );
		m_components.Get<ecsScaling>().vector.reserve(	512 );
		m_components.Get<ecsAABB>().vector.reserve(		512 );

		m_components.Get<ecsSphereShape>().vector.reserve( 2 );//TMP

		size_t capa0 = m_components.Get<ecsRigidbody>().vector.capacity();
		m_components.Get<ecsRigidbody>().vector.reserve(3);
		size_t capa1 = m_components.Get<ecsRigidbody>().vector.capacity();
		(void )capa0; (void)capa1;

		m_components.Get<ecsMotionState>().vector.reserve( 2 );
		

		m_components.Get<ecsRigidbody>().onPreRealloc.Connect( &Signal<>::Emmit, &onPreReallocPhysics );
		m_components.Get<ecsMotionState>().onPreRealloc.Connect( &Signal<>::Emmit, &onPreReallocPhysics );
		m_components.Get<ecsSphereShape>().onPreRealloc.Connect( &Signal<>::Emmit, &onPreReallocPhysics );
		m_components.Get<ecsBoxShape>().onPreRealloc.Connect( &Signal<>::Emmit, &onPreReallocPhysics );
		
		m_components.Get<ecsRigidbody>().onPostRealloc.Connect( &Signal<>::Emmit, &onPostReallocPhysics );
		m_components.Get<ecsMotionState>().onPostRealloc.Connect( &Signal<>::Emmit, &onPostReallocPhysics );
		m_components.Get<ecsSphereShape>().onPostRealloc.Connect( &Signal<>::Emmit, &onPostReallocPhysics );
		m_components.Get<ecsBoxShape>().onPostRealloc.Connect( &Signal<>::Emmit, &onPostReallocPhysics );

	}

	//================================================================================================================================
	//================================================================================================================================
	ecsEntity EcsManager::CreateEntity() {
		if ( m_entitiesData.size() == m_entitiesData.capacity() ) {
			m_entitiesData.reserve( 2 * m_entitiesData.size() );
			Debug::Log("realloc entities");
		}
		ecsEntity entity = static_cast<ecsEntity>( m_entitiesData.size());
		m_entitiesData.push_back( ecsEntityData() );
		return static_cast<ecsEntity> ( entity );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EcsManager::DeleteEntity( const ecsEntity  _entity ) {
		m_entitiesData[_entity].Kill();
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
		static void Run( const float _delta, const size_t _count, std::vector<ecsEntityData>& _entitiesData, ecsComponentsTuple< ecsComponents >& _tuple ) {
			_system::Run( _delta, _count, _entitiesData,  _tuple.Get<_types>().vector ... );
		}
	};

	//================================================================================================================================
	// Runs the systems before the physics update
	//================================================================================================================================
	void EcsManager::Update( float _delta ) {
		RunSystem< ParticleSystem::signature::componentsTypes, ParticleSystem >::Run( _delta, m_activeEntitiesCount, m_entitiesData, m_components );
		RunSystem< PlanetsSystem::signature::componentsTypes, PlanetsSystem >::Run( _delta, m_activeEntitiesCount, m_entitiesData, m_components );
		RunSystem< SynchRbToTransSystem::signature::componentsTypes, SynchRbToTransSystem >::Run( _delta, m_activeEntitiesCount, m_entitiesData, m_components );
	}

	//================================================================================================================================
	// Runs the systems after the physics update
	//================================================================================================================================
	void EcsManager::LateUpdate( float _delta ) {
		RunSystem< SynchTransToRbSystem::signature::componentsTypes, SynchTransToRbSystem >::Run( _delta, m_activeEntitiesCount, m_entitiesData, m_components );
	}

	//================================================================================================================================
	// Helper for the RecycleComponent method
	// iterates recursively on static components indices, finds the on that matches the runtime index _id and
	// appends _componentIndex to the corresponding recycle list
	//================================================================================================================================
	class RecycleHelper {
	private:
		// General case
		template< size_t _index >
		static void RecycleImpl( ecsComponentsTuple< ecsComponents >& _components, size_t _id, uint32_t _componentIndex ) {
			if ( _index == _id ) {
				_components.Get<_index>().recycleList.push_back( _componentIndex );
			} else {
				RecycleImpl< _index - 1>( _components, _id, _componentIndex );
			}
		}

		// Specialization 
		template< >
		static void RecycleImpl<0>( ecsComponentsTuple< ecsComponents >& _components, size_t _id, uint32_t _componentIndex ) {
			if ( _id == 0 ) {
				_components.Get<0>().recycleList.push_back( _componentIndex );
			} else {
				assert( false ); // Out of range
			}
		}
	public:
		static void Recycle( ecsComponentsTuple< ecsComponents >& _components, size_t _id, uint32_t _componentIndex ) {
			RecycleImpl< ecsComponents::count - 1 >( _components, _id, _componentIndex );
		}
	};	   

	//================================================================================================================================
	// put _componentIndex in the recycleList of component _componentID
	//================================================================================================================================
	void EcsManager::RecycleComponent( const uint32_t _componentID, const uint32_t _componentIndex ) {
		assert( _componentID  < ecsComponents::count );
		RecycleHelper::Recycle( m_components, _componentID, _componentIndex );
	}

	//================================================================================================================================
	// Removes the dead entities at the end of the entity vector
	//================================================================================================================================
	void EcsManager::RemoveDeadEntities() {
		if ( m_entitiesData.empty() ) {
			return;
		}

		int64_t reverseIndex = m_entitiesData.size() - 1;
		while ( reverseIndex >= 0 ) {
			ecsEntityData& data = m_entitiesData[reverseIndex];
			if ( data.IsAlive() ) {
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
				if( data.bitset[componentID]) {
					RecycleComponent( componentID, data.components[componentID] );
				}
			}
			m_entitiesData.pop_back();
			--reverseIndex;
		}
		m_activeEntitiesCount = static_cast<ecsEntity>( m_entitiesData.size() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void  EcsManager::RemoveDeadComponentsAndTags() {
		// Remove components
		for ( std::pair< ecsEntity, uint32_t> & pair : m_removedComponents ) {
			const ecsEntity entity = pair.first;
			const  uint32_t componentIndex = pair.second;
			if( m_entitiesData[entity].bitset[componentIndex] == 1 ) {
				m_entitiesData[entity].bitset[componentIndex] = 0;
				RecycleComponent( componentIndex, m_entitiesData[entity].components[componentIndex] );
			} else {
				Debug::Get() << Debug::Severity::warning << "Remove component failed : Entity "<< entity << " has no component " << componentIndex << Debug::Endl();
			}
		} m_removedComponents.clear();

		// Removes tags
		for ( std::pair< ecsEntity, uint32_t> & pair : m_removedTags ) {
			const ecsEntity entity = pair.first;
			const  uint32_t tagIndex = pair.second;
			if ( m_entitiesData[entity].bitset[tagIndex] == 1 ) {
				m_entitiesData[entity].bitset[tagIndex] = 0;			
			} else {
				Debug::Get() << Debug::Severity::warning << "Remove tag failed : Entity " << entity << " has no component " << tagIndex << Debug::Endl();
			}
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
		const int64_t size = static_cast<int64_t>(m_entitiesData.size());
		int64_t forwardIndex = 0;
		int64_t reverseIndex = m_entitiesData.size() - 1;

		while( true ) {
			while ( forwardIndex < size - 1 && m_entitiesData[forwardIndex].IsAlive()  ) { ++forwardIndex; } // Finds a dead entity
			if( forwardIndex == size - 1 ) break;

			while ( reverseIndex > 0 && m_entitiesData[reverseIndex].IsDead() )   { --reverseIndex; } // Finds a live entity
			if ( reverseIndex == 0 ) break;

			if ( forwardIndex > reverseIndex  ) break;

			// updates the indices of the corresponding handle if necessary
			SwapHandlesEntities((ecsEntity) reverseIndex ,(ecsEntity) forwardIndex );

			std::swap( m_entitiesData[reverseIndex] , m_entitiesData[forwardIndex] );	
			++forwardIndex; --reverseIndex;
		}
	}
}