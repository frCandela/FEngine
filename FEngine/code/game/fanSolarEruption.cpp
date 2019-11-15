#include "fanGlobalIncludes.h"
#include "game/fanSolarEruption.h"

#include "game/fanPlanet.h"
#include "core/time/fanProfiler.h"
#include "renderer/fanRenderer.h"
#include "renderer/fanMesh.h"
#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanMeshRenderer.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanMaterial.h"
#include "scene/actors/fanParticleSystem.h"
#include "editor/fanModals.h"

namespace fan {
	REGISTER_TYPE_INFO( SolarEruption, TypeInfo::Flags::EDITOR_COMPONENT )

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::Start() {
		REQUIRE_COMPONENT( Material, m_material )
		REQUIRE_COMPONENT( ParticleSystem, m_particleSystem )
		REQUIRE_COMPONENT( MeshRenderer, m_meshRenderer )


		if ( m_particleSystem )	{ m_particleSystem->SetEnabled( false ); }
		if ( m_meshRenderer )	{ m_meshRenderer->SetMesh( ecsSolarEruptionMeshSystem::s_mesh );}		
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::OnAttach() {
		Actor::OnAttach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::OnDetach() {
		Actor::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::Update(const float _delta) {
		SCOPED_PROFILE( solar_erup );
		UpdateStateMachine( _delta );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::UpdateStateMachine( const float _delta ) {
		SCOPED_PROFILE( state );

		if ( m_state != NONE ) {
			m_eruptionTime += _delta;

			switch ( m_state ) {
			case COLLAPSING: {
				const float scale = m_eruptionTime / m_warmingTime; // between 0-1

				// Color
				Color color = m_baseColor;
				color[3] *= 1.f - scale;
				m_material->SetColor( color );

				// Particles
				m_particleSystem->SetParticlesPerSecond( int( scale * m_warmingMaxParticles ) );

				if ( m_eruptionTime > m_warmingTime ) {
					m_eruptionTime -= m_warmingTime;
					m_state = WAITING;
				}
			} break;
			case WAITING: {
				const float scale = m_eruptionTime / m_waitingTime; // between 0-1

				// Particles
				m_particleSystem->SetParticlesPerSecond( int( scale * ( m_waitingMaxParticles - m_warmingMaxParticles ) ) );

				if ( m_eruptionTime > m_waitingTime ) {
					m_eruptionTime -= m_waitingTime;
					m_state = EXPODING;
				}
			} break;
			case EXPODING: {
				const float scale = std::clamp ( m_eruptionTime / m_explosionFadeinTime, 0.f, 1.f );

				// Particles
				m_particleSystem->SetParticlesPerSecond( int( m_explosionMaxParticles ) );
				m_particleSystem->SetSpeedParticles( m_explosionParticlesSpeed );

				//scale
				m_gameobject->GetTransform()->SetScale( btVector3( scale, 1.f, scale ) );

				// Color
				m_material->SetColor( m_explositonColor );

				if ( m_eruptionTime > m_explosionTime ) {
					m_eruptionTime -= m_explosionTime;
					m_state = BACK_TO_NORMAL;
				}
			} break;
			case BACK_TO_NORMAL: {
				const float scale = m_eruptionTime / m_backToNormalTime;

				m_material->SetColor( ( 1.f - scale ) * m_explositonColor.ToGLM() + scale * m_baseColor.ToGLM() );
				m_particleSystem->SetParticlesPerSecond( int( ( 1.f - scale ) * m_explosionMaxParticles ) );

				if ( m_eruptionTime > m_backToNormalTime ) {
					m_eruptionTime -= m_backToNormalTime;
					m_state = NONE;

					m_particleSystem->SetEnabled( false );
					m_material->SetColor( m_baseColor );
				}
			} break;
			default: {
				m_state = NONE;
			} break;
			}
		}
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::OnGui()
	{

		ImGui::PushItemWidth( 200.f );
		{
			// State machine		
			ImGui::DragFloat( "eruption time", &m_eruptionTime );
			ImGui::DragFloat( "warming time", &m_warmingTime );
			ImGui::DragFloat( "warming max particles", &m_warmingMaxParticles );
			ImGui::DragFloat( "waiting time", &m_waitingTime );
			ImGui::DragFloat( "waiting max particles", &m_waitingMaxParticles );
			ImGui::DragFloat( "explosion time", &m_explosionTime );
			ImGui::DragFloat( "explosion fade-in time", &m_explosionFadeinTime );
			ImGui::DragFloat( "explosion max particles", &m_explosionMaxParticles );
			ImGui::DragFloat( "explosion particles speed", &m_explosionParticlesSpeed );
			ImGui::DragFloat( "back to normal time", &m_backToNormalTime );
			ImGui::Spacing();
		} ImGui::PopItemWidth();

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			// Colors
			ImGui::Spacing();
			if ( ImGui::ColorEdit4( "base ", m_baseColor.Data(), ImGui::fanColorEditFlags ) )
			{
				m_material->SetColor( m_baseColor );
			}
			ImGui::ColorEdit4( "explositon ", m_explositonColor.Data(), ImGui::fanColorEditFlags );

			// Test
			ImGui::Spacing();
			if ( ImGui::Button( "Eruption" ) )
			{
				m_state = COLLAPSING;
				m_eruptionTime = 0.f;
				m_particleSystem->SetEnabled( true );
				m_material->SetColor( m_baseColor );
				m_particleSystem->SetSpeedParticles( 1.f );
			}
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SolarEruption::Load( Json & _json) {
		Actor::Load(_json);

		// State machine		
		LoadFloat( _json, "eruption time", m_eruptionTime );
		LoadFloat( _json, "warming time", m_warmingTime );
		LoadFloat( _json, "warming max particles", m_warmingMaxParticles );
		LoadFloat( _json, "waiting time", m_waitingTime );
		LoadFloat( _json, "waiting max particles", m_waitingMaxParticles );
		LoadFloat( _json, "explosion time", m_explosionTime );
		LoadFloat( _json, "explosion fade-in time", m_explosionFadeinTime );
		LoadFloat( _json, "explosion max particles", m_explosionMaxParticles );
		LoadFloat( _json, "explosion particles speed", m_explosionParticlesSpeed );
		
		// Colors
		LoadColor( _json, "base ", m_baseColor );
		LoadColor( _json, "explositon ", m_explositonColor );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SolarEruption::Save( Json & _json ) const {
		Actor::Save( _json );		

		// State machine		
		SaveFloat( _json, "eruption time", m_eruptionTime );
		SaveFloat( _json, "warming time", m_warmingTime );
		SaveFloat( _json, "warming max particles", m_warmingMaxParticles );
		SaveFloat( _json, "waiting time", m_waitingTime );
		SaveFloat( _json, "waiting max particles", m_waitingMaxParticles );
		SaveFloat( _json, "explosion time", m_explosionTime );
		SaveFloat( _json, "explosion fade-in time", m_explosionFadeinTime );
		SaveFloat( _json, "explosion max particles", m_explosionMaxParticles );
		SaveFloat( _json, "explosion particles speed", m_explosionParticlesSpeed );

		// Colors
		SaveColor( _json, "base ", m_baseColor );
		SaveColor( _json, "explositon ", m_explositonColor );
		return true;
	}
}