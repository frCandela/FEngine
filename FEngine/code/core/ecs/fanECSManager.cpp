#include "fanGlobalIncludes.h"
#include "core/ecs/fanECSManager.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	EcsManager::EcsManager() {
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
		m_entitiesData.push_back( ecsEntityData() );
		return static_cast<ecsEntity> (m_entitiesData.size() - 1 );
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

	}

	//================================================================================================================================
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

			// This is ugly but there is no way to access tuple data with runtime indices... TODO Make a macro or a variadic template struct
			for (int componentIndex = 0; componentIndex < ecsComponents::count ; componentIndex++) {
				if( data.bitset[componentIndex])
				switch ( componentIndex ) {
				case 0:	m_components.Get<0>().recycleList.push_back( componentIndex ); break;
				case 1:	m_components.Get<1>().recycleList.push_back( componentIndex ); break;
				case 2:	m_components.Get<2>().recycleList.push_back( componentIndex ); break;
				case 3:	m_components.Get<3>().recycleList.push_back( componentIndex ); break;
				default:
					assert( false);
					break;
				}
			}
			m_entitiesData.pop_back();
			--reverseIndex;
		}
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

		ImGui::Text( ( "Entities Count: " + std::to_string(m_entitiesData.size())).c_str() );
		ImGui::Text( TagCountSize( "Storage size:   ", m_entitiesData.capacity(), sizeof( ecsEntityData ) ).c_str() );

		ImGui::Separator();

		auto& dataTransform = m_components.Get< ecsTranform >();
		ImGui::Text( TagCountSize( "CTranform:      ", dataTransform.vector.size() - dataTransform.recycleList.size(),	sizeof( ecsTranform ) ).c_str() );
		auto& dataMovement = m_components.Get< ecsMovement >();
		ImGui::Text( TagCountSize( "SCMovement:     ", dataMovement.vector.size() - dataMovement.recycleList.size(), sizeof( ecsMovement ) ).c_str() );
		auto& dataParticle = m_components.Get< ecsParticle >();
		ImGui::Text( TagCountSize( "CParticle:         ", dataParticle.vector.size() - dataParticle.recycleList.size(),		sizeof( ecsParticle ) ).c_str() );
		auto& dataScale = m_components.Get< ecsScaling >();
		ImGui::Text( TagCountSize( "CScale:         ", dataScale.vector.size() - dataScale.recycleList.size(), sizeof( ecsScaling ) ).c_str() );


		ImGui::Separator();
		ImGui::Separator();

		static bool s_transform, s_particle, s_movement, s_scale;
		ImGui::Checkbox("transform",&s_transform );
		ImGui::SameLine (); ImGui::Checkbox( "movement", &s_movement );
		ImGui::SameLine (); ImGui::Checkbox( "particle", &s_particle );
		ImGui::SameLine (); ImGui::Checkbox( "scale", &s_scale );
		static bool s_fakeTag;
		ImGui::Checkbox( "fakeTag", &s_fakeTag );

		static int nb = 1;
		if ( ImGui::Button( "Create Entities") ) {
			for (int i = 0; i < nb; i++) {		
				ecsEntity entity = CreateEntity();
				if ( s_transform )	AddComponent<ecsTranform>( entity );
				if ( s_movement )	AddComponent<ecsMovement>( entity );
				if ( s_particle )		AddComponent<ecsParticle>( entity );
				if ( s_scale )		AddComponent<ecsScaling>( entity );
				if ( s_fakeTag )	AddTag<ecsFakeTag>( entity );
			}
		} ImGui::SameLine (); ImGui::PushItemWidth(100);
		ImGui::DragInt( "nb", &nb, 1, 1, 100000 );

		static int entitySelect = 0;
		if ( ImGui::Button( "Create handle" ) ) {
			CreateHandle( entitySelect );
		} ImGui::SameLine();
		ImGui::DragInt( "handleIndex", &entitySelect );

		if ( ImGui::Button( "Refresh" ) ) { Refresh(); } ImGui::SameLine();
		if ( ImGui::Button( "Sort" ) ) { SortEntities();	} ImGui::SameLine();
		if ( ImGui::Button( "RemoveDead" ) ) { RemoveDeadEntities(); }

		// Entities list
		if ( ImGui::CollapsingHeader( "Entities" ) ) {
			for ( int entityIndex = 0; entityIndex < m_entitiesData.size(); entityIndex++ ) {
				ecsEntityData & data = m_entitiesData[entityIndex];	
				ImGui::PushID( entityIndex );
				if( ImGui::Button("X##killentity") ) {
					data.Kill();
				} ImGui::SameLine();
				ImGui::Text( data.bitset.to_string().c_str() );
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