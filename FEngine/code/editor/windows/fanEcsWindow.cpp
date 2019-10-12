#include "fanGlobalIncludes.h"
#include "editor/windows/fanEcsWindow.h"

#include "core/ecs/fanEcsManager.h"

namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	EcsWindow::EcsWindow( EcsManager * _ecsManager ) :
		EditorWindow("ecs")
		, m_ecsManager( _ecsManager )		
		,m_generator()					 // TMP
		, m_distribution( 0.f, 1.f )	// TMP
	{
	}

	//================================================================
	// helper function to create a formatted string like "Storage: 1024 (16Ko)"
	//================================================================
	std::string TagCountSize( const char * _tag, const size_t _count, const size_t _size ) {
		std::stringstream ssStorage;
		ssStorage << _tag << ": " << _count << " (" << _count * _size / 1000 << "Ko)";
		return ssStorage.str();
	}

	//================================================================================================================================
	// Helper to draw all the components data
	//================================================================================================================================
	template <typename _componentsList > class DisplayHelper;
	template < template <typename...> typename _componentsList, typename... _components >
	class DisplayHelper<_componentsList<_components...> > {
	public:

		template<typename ... _list > struct DisplayImpl;

		// Specialization 
		template<> struct DisplayImpl<> {
			static void Display( const ecsComponentsTuple< ecsComponents >& /*_tuple*/ ) {}
		};

		// General case
		template<typename _component, typename ... _list>
		struct DisplayImpl< _component, _list...> : DisplayImpl<_list...> {
			static_assert( IsComponent<_component>::value );
			static void Display( const ecsComponentsTuple< ecsComponents >& _tuple ) {
				const auto& dataTransform = _tuple.Get< _component >();
				ImGui::Text( TagCountSize( _component::s_name, dataTransform.vector.size() - dataTransform.recycleList.size(), sizeof( _component ) ).c_str() );
				DisplayImpl<_list...>::Display( _tuple );
			}
		};

		static void Display( const ecsComponentsTuple< ecsComponents >& _tuple ) {
			DisplayImpl<_components...>::Display( _tuple );
		}
	};

	//================================================================================================================================
	//================================================================================================================================
	void EcsWindow::OnGui() {
		const std::vector<ecsEntityData>& entitiesData = m_ecsManager->GetEntitiesData();
		const ecsComponentsTuple< ecsComponents >& components = m_ecsManager->GetComponents();
		const std::unordered_map< ecsHandle, ecsEntity > & handlesToEntity = m_ecsManager->GetHandles();


		// Entities info

		ImGui::Text( TagCountSize( "Entities  ", entitiesData.size(), sizeof( ecsEntityData ) ).c_str() );

		ImGui::Separator();

		// Components Info
		DisplayHelper<ecsComponents>::Display( components );

		// Test
		if ( ImGui::CollapsingHeader( "Testing" ) ) {
			// Components & tags selection
			static bool s_transform, s_particle, s_movement, s_scale;
			ImGui::Checkbox( "transform", &s_transform );
			ImGui::SameLine (); ImGui::Checkbox( "movement", &s_movement );
			ImGui::SameLine (); ImGui::Checkbox( "particle", &s_particle );
			ImGui::SameLine (); ImGui::Checkbox( "scale", &s_scale );
			static bool s_fakeTag;
			ImGui::Checkbox( "fakeTag", &s_fakeTag );

			// Entity creation
			ImGui::PushItemWidth( 100 );
			static int nb = 1;
			if ( ImGui::Button( "Create Entities" ) ) {
				for ( int i = 0; i < nb; i++ ) {
					ecsEntity entity =  m_ecsManager->CreateEntity();
					if ( s_transform )   m_ecsManager->AddComponent<ecsTranform>( entity );
					if ( s_movement ) 	 m_ecsManager->AddComponent<ecsMovement>( entity );
					if ( s_particle )	 m_ecsManager->AddComponent<ecsParticle>( entity );
					if ( s_scale )		 m_ecsManager->AddComponent<ecsScaling>( entity );
					if ( s_fakeTag )	 m_ecsManager->AddTag<ecsFakeTag>( entity );
				}
			} ImGui::SameLine (); 
			ImGui::DragInt( "nb", &nb, 1, 1, 100000 );

			// Create handle
			static int entitySelect = 0;
			if ( ImGui::Button( "Create handle  " ) ) {
				m_ecsManager->CreateHandle( entitySelect );
			} ImGui::SameLine();
			ImGui::DragInt( "handleIndex", &entitySelect );
			ImGui::PopItemWidth();
		}

		// PARTICLES
		if ( ImGui::CollapsingHeader( "Particles" ) ) {
			static int particlesPerFrame = 1;
			static float speed = 1.f;
			static float duration = 2.f;
			static btVector3 startPos;

			ImGui::DragInt( "particlesPerFrame", &particlesPerFrame, 1, 0 );
			ImGui::DragFloat( "speed", &speed, 0.01f );
			ImGui::DragFloat( "duration", &duration, 0.01f );
			ImGui::DragFloat3( "start pos", &startPos[0] );

			if ( particlesPerFrame > 0 ) {
				for ( int particleIndex = 0; particleIndex < particlesPerFrame; particleIndex++ ) {
					ecsEntity entity = m_ecsManager->CreateEntity();
					m_ecsManager->AddComponent<ecsTranform>( entity );
					m_ecsManager->AddComponent<ecsMovement>( entity );
					m_ecsManager->AddComponent<ecsParticle>( entity );

					ecsTranform* transform = m_ecsManager->FindComponentFromEntity<ecsTranform>( entity );
					ecsMovement* movement = m_ecsManager->FindComponentFromEntity<ecsMovement>( entity );
					ecsParticle* particle = m_ecsManager->FindComponentFromEntity<ecsParticle>( entity );

					movement->speed = btVector3( m_distribution( m_generator ), m_distribution( m_generator ), m_distribution( m_generator ) ) - btVector3( 0.5f, 0.5f, 0.5f );
					movement->speed.normalize();
					movement->speed *= speed;
					transform->position = startPos;
					particle->durationLeft = duration;
				}
			}
		}

		// Entities list
		if ( ImGui::CollapsingHeader( "Entities" ) ) {
			for ( int entityIndex = 0; entityIndex < entitiesData.size(); entityIndex++ ) {
				const ecsEntityData & data = entitiesData[entityIndex];
				std::stringstream ss;
				ss << data.bitset.to_string() << " " << entityIndex;
				ImGui::Text( ss.str().c_str() );
			}
		}

		// Entities handles
		if ( ImGui::CollapsingHeader( "Handles" ) ) {
			for ( auto& handlePair : handlesToEntity ) {
				ImGui::Text( ( std::to_string( handlePair.first ) + " " + std::to_string( handlePair.second ) ).c_str() );
			}
		}		
	}
}