#include "fanGlobalIncludes.h"
#include "core/ecs/fanECSManager.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	EcsManager::EcsManager() {
		m_entitiesData.reserve(1024);
	}

	//================================================================================================================================
	//================================================================================================================================
	CEntity EcsManager::CreateEntity() {
		if ( m_entitiesData.size() + 1 >= m_entitiesData.capacity() ) {
			m_entitiesData.reserve( 2 * m_entitiesData.size() );
			Debug::Log("realloc");
		}
		m_entitiesData.push_back( EntityData() );
		return static_cast<CEntity> (m_entitiesData.size() - 1 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EcsManager::DeleteEntity( const CEntity  _entity ) {
		m_entitiesData[_entity].Kill();
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

// 		int reverseIndex = m_entitiesData.size() - 1;
// 		while ( m_entitiesData[reverseIndex].IsDead() ) {
// 
// 		}
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
		ImGui::Text( TagCountSize( "Storage size:   ", m_entitiesData.capacity(), sizeof( EntityData ) ).c_str() );

		ImGui::Separator();

		ImGui::Text( TagCountSize( "CTranform:      ", m_components.Get< CTranform >().size(),	sizeof( CTranform ) ).c_str() );
		ImGui::Text( TagCountSize( "SCMovement:     ", m_components.Get< CMovement >().size(),	sizeof( CMovement ) ).c_str() );
		ImGui::Text( TagCountSize( "CColor:         ", m_components.Get< CColor >().size(),		sizeof( CColor ) ).c_str() );

		ImGui::Separator();
		ImGui::Separator();

		static bool s_transform, s_color, s_movement;		
		ImGui::Checkbox("transform",&s_transform );
		ImGui::SameLine (); ImGui::Checkbox( "movement", &s_movement );
		ImGui::SameLine (); ImGui::Checkbox( "color", &s_color );
		static bool s_ally, s_ennemy;
		ImGui::Checkbox( "ally", &s_ally ); 
		ImGui::SameLine (); ImGui::Checkbox( "ennemy", &s_ennemy );

		static int nb = 1;
		if ( ImGui::Button( "Create Entities") ) {


			for (int i = 0; i < nb; i++) {		
				CEntity entity = CreateEntity();
				if ( s_transform )	AddComponent<CTranform>( entity );
				if ( s_movement )	AddComponent<CMovement>( entity );
				if ( s_color )		AddComponent<CColor>( entity );
				if ( s_ally )		AddTag<TAlly>( entity );
				if ( s_ennemy )		AddTag<TEnnemy>( entity );
			}
		} ImGui::SameLine (); ImGui::PushItemWidth(100);
		ImGui::DragInt( "nb", &nb, 1, 1, 100000 );

		if ( ImGui::Button( "Refresh" ) ) { Refresh(); } ImGui::SameLine();
		if ( ImGui::Button( "Sort" ) ) { SortEntities();	} ImGui::SameLine();
		if ( ImGui::Button( "RemoveDead" ) ) { RemoveDeadEntities(); }

		if ( ImGui::CollapsingHeader( "Entities" ) ) {
			for ( int entityIndex = 0; entityIndex < m_entitiesData.size(); entityIndex++ ) {
				EntityData & data = m_entitiesData[entityIndex];	
				ImGui::PushID( entityIndex );
				if( ImGui::Button("X##killentity") ) {
					data.Kill();
				} ImGui::SameLine();
				ImGui::Text( data.bitset.to_string().c_str() );
				ImGui::PopID();
			}
		}

		ImGui::End();
	}
}