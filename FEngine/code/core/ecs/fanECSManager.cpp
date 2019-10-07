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
		if ( m_lastEntity + 1 >= m_entitiesData.size() ) {
			m_entitiesData.reserve( 2 * m_entitiesData.size() );
		}
		m_entitiesData.push_back(EntityData());
		return m_lastEntity++;
	}

	//================================================================================================================================
	//================================================================================================================================
	void EcsManager::DeleteEntity( const CEntity  _entity ) {
		m_entitiesData[_entity].Kill();
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
		static int nb = 1;
		if ( ImGui::Button( "Create Entities" ) ) {
			for (int i = 0; i < nb; i++) {
				CreateEntity();
			}
			
		} ImGui::SameLine (); ImGui::PushItemWidth(100);
		ImGui::DragInt( "nb", &nb, 1, 1, 100000 );

		if ( ImGui::CollapsingHeader( "Entities" ) ) {
			for ( int entityIndex = 0; entityIndex < m_entitiesData.size(); entityIndex++ ) {
				EntityData & data = m_entitiesData[entityIndex];
				std::stringstream ss;
				ss << data.GetBitset().to_string() << " " << entityIndex;
				ImGui::Text( ss.str().c_str() );
			}
		}

		ImGui::End();
	}
}