#include "fanGlobalIncludes.h"
#include "core/ecs/fanECSManager.h"
#include "core/fanTime.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	EcsManager::EcsManager() :	
		m_generator()
		,m_distribution( 0.f, 1.f ) {
		m_entityToHandles.reserve( 512 );
		m_entitiesData.reserve(1024);
	}

	//================================================================================================================================
	//================================================================================================================================
	ecsEntity EcsManager::CreateEntity() {
		if ( m_entitiesData.size() + 1 >= m_entitiesData.capacity() ) {
			m_entitiesData.reserve( 2 * m_entitiesData.size() );
			Debug::Log("realloc");
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
	//================================================================================================================================
	void EcsManager::Refresh() {
		if( m_enableRefresh ) {
			RemoveDeadComponentsAndTags();
			SortEntities();
			RemoveDeadEntities();
		}
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
	//================================================================================================================================
	void EcsManager::Update( float _delta ) {
		if ( m_enableUpdate ) {
			RunSystem< ParticleSystem::signature::componentsTypes, ParticleSystem >::Run( _delta, m_activeEntitiesCount, m_entitiesData, m_components );
		}
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
			if ( _id == _id ) {
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

	//================================================================
	// helper function to create a formatted string like "Storage: 1024 (16Ko)"
	//================================================================
	std::string TagCountSize( const char * _tag, const size_t _count, const size_t _size ) {
		std::stringstream ssStorage;
		ssStorage << _tag << _count << " (" << _count * _size / 1000 << "Ko)";
		return ssStorage.str();
	}

	//================================================================================================================================
 	//================================================================================================================================
 	void EcsManager::OnGui() {
		ImGui::Begin("ECS");

		// Entities info
		ImGui::Text( ( "Entities Count: " + std::to_string(m_entitiesData.size())).c_str() );
		ImGui::Text( TagCountSize( "Storage size:   ", m_entitiesData.capacity(), sizeof( ecsEntityData ) ).c_str() );

		ImGui::Separator();

		// Components Info
		auto& dataTransform = m_components.Get< ecsTranform >();
		ImGui::Text( TagCountSize( "CTranform:      ", dataTransform.vector.size() - dataTransform.recycleList.size(),	sizeof( ecsTranform ) ).c_str() );
		auto& dataMovement = m_components.Get< ecsMovement >();
		ImGui::Text( TagCountSize( "SCMovement:     ", dataMovement.vector.size() - dataMovement.recycleList.size(), sizeof( ecsMovement ) ).c_str() );
		auto& dataParticle = m_components.Get< ecsParticle >();
		ImGui::Text( TagCountSize( "CParticle:      ", dataParticle.vector.size() - dataParticle.recycleList.size(),		sizeof( ecsParticle ) ).c_str() );
		auto& dataScale = m_components.Get< ecsScaling >();
		ImGui::Text( TagCountSize( "CScale:         ", dataScale.vector.size() - dataScale.recycleList.size(), sizeof( ecsScaling ) ).c_str() );

		ImGui::Separator();
		ImGui::Separator();

		// Components & tags selection
		static bool s_transform, s_particle, s_movement, s_scale;
		ImGui::Checkbox("transform",&s_transform );
		ImGui::SameLine (); ImGui::Checkbox( "movement", &s_movement );
		ImGui::SameLine (); ImGui::Checkbox( "particle", &s_particle );
		ImGui::SameLine (); ImGui::Checkbox( "scale", &s_scale );
		static bool s_fakeTag;
		ImGui::Checkbox( "fakeTag", &s_fakeTag );

		// Entity creation
		static int nb = 1;
		if ( ImGui::Button( "Create Entities") ) {
			for (int i = 0; i < nb; i++) {		
				ecsEntity entity = CreateEntity();
				if ( s_transform )  AddComponent<ecsTranform>( entity ); 
				if ( s_movement ) 	AddComponent<ecsMovement>( entity ); 
				if ( s_particle )	AddComponent<ecsParticle>( entity );
				if ( s_scale )		AddComponent<ecsScaling>( entity );
				if ( s_fakeTag )	AddTag<ecsFakeTag>( entity );
			}
		} ImGui::SameLine (); ImGui::PushItemWidth(100);
		ImGui::DragInt( "nb", &nb, 1, 1, 100000 );

		// Create handle
		static int entitySelect = 0;
		if ( ImGui::Button( "Create handle" ) ) {
			CreateHandle( entitySelect );
		} ImGui::SameLine();
		ImGui::DragInt( "handleIndex", &entitySelect );

		// Entity management
		int tempActiveCount = (int)m_activeEntitiesCount; ImGui::DragInt( "m_activeEntitiesCount", &tempActiveCount );
		if ( ImGui::Button( "Update" ) ) { Update( 0.f ); } ImGui::SameLine();
		if ( ImGui::Button( "Sort" ) ) { SortEntities();	} ImGui::SameLine();
		if ( ImGui::Button( "RemoveDead" ) ) { RemoveDeadEntities(); }ImGui::SameLine();
		ImGui::Checkbox("disable refresh", &m_enableRefresh ); ImGui::SameLine();
		ImGui::Checkbox( "disable update", &m_enableUpdate );
		
		// PARTICLES
		static int particlesPerFrame = 1;
		static float speed = 1.f;
		static float duration = 2.f;
		static btVector3 startPos;
		if ( ImGui::CollapsingHeader( "Particles" ) ) {
			ImGui::DragInt("particlesPerFrame", &particlesPerFrame, 1, 0 );
			ImGui::DragFloat( "speed", &speed, 0.01f);
			ImGui::DragFloat( "duration", &duration, 0.01f );
			ImGui::DragFloat3( "start pos", &startPos[0]);

			if ( particlesPerFrame > 0 ) {
				for ( int particleIndex = 0; particleIndex < particlesPerFrame; particleIndex++ ) {
					ecsEntity entity = CreateEntity();
					ecsTranform& transform = m_components.Get<ecsTranform>().vector[AddComponent<ecsTranform>( entity )];
					ecsMovement& movement = m_components.Get<ecsMovement>().vector[AddComponent<ecsMovement>( entity )];
					ecsParticle& particle = m_components.Get<ecsParticle>().vector[AddComponent<ecsParticle>( entity )];
					
					movement.speed = btVector3( m_distribution( m_generator ), m_distribution( m_generator ), m_distribution( m_generator ) ) - btVector3(0.5f, 0.5f, 0.5f);
					movement.speed.normalize();
					movement.speed *= speed;
					transform.position = startPos;
					particle.durationLeft = duration;
				}
			}
		}

		// Entities list
		if ( ImGui::CollapsingHeader( "Entities" ) ) {
			for ( int entityIndex = 0; entityIndex < m_entitiesData.size(); entityIndex++ ) {
				ecsEntityData & data = m_entitiesData[entityIndex];	
				ImGui::PushID( entityIndex );
				if( ImGui::Button("X##killentity") ) {
					data.Kill();
				} ImGui::SameLine();
				
				std::stringstream ss;
				ss << data.bitset.to_string()<< " " << entityIndex;
				ImGui::Text( ss.str().c_str());

				ImGui::PopID();
			}
		}

		// Entities handles
		if ( ImGui::CollapsingHeader( "Handles" ) ) {					
			for ( auto& handlePair : m_handlesToEntity ) {
				ImGui::Text( ( std::to_string( handlePair.first ) + " " + std::to_string( handlePair.second ) ).c_str() );
			}
			ImGui::Separator();
			for ( auto& handlePair : m_entityToHandles ) {
				ImGui::Text( ( std::to_string( handlePair.first ) + " " + std::to_string( handlePair.second ) ).c_str() );
			}
		}
		ImGui::End();
	}
}