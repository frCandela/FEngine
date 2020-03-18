#include "game/components/fanSolarEruption.hpp"
#include "game/components/fanPlanet.hpp"
#include "game/components/fanPlayersManager.hpp"
#include "game/components/fanSolarPanel.hpp"
#include "game/components/fanHealth.hpp"
#include "core/time/fanProfiler.hpp"
#include "editor/fanModals.hpp"
#include "render/fanMesh.hpp"

namespace fan
{

		//================================================================================================================================
		//================================================================================================================================
		void SolarEruption::Start()
	{
//		REQUIRE_COMPONENT( Material, m_material );
//		REQUIRE_COMPONENT( ParticleSystem, m_particleSystem );
// 		REQUIRE_COMPONENT( MeshRenderer, m_meshRenderer );
// 		REQUIRE_SCENE_COMPONENT( PlayersManager, m_players );
//
// 		if( m_particleSystem )
// 		{
// 			m_particleSystem->SetEnabled( false );
// 		}
// 		if( m_meshRenderer )
// 		{
// 			// Get singleton components 
// 			ecsSunLightMesh_s& sunLight = GetScene().GetEcsManager().GetSingletonComponents().GetComponent<ecsSunLightMesh_s>();
// 			m_meshRenderer->SetMesh( sunLight.mesh );
// 		}
// 
// 		m_timeBeforeEruption = m_eruptionCooldown + m_distribution( m_generator ) * m_eruptionRandomCooldown;
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::OnAttach()
	{


		m_distribution = std::uniform_real_distribution<float>( 0.f, 1.f );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::OnDetach()
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::Update( const float _delta )
	{
		SCOPED_PROFILE( solar_erup );
		UpdateStateMachine( _delta );

		m_timeBeforeEruption -= _delta;
		if ( m_timeBeforeEruption < 0.f )
		{
			m_timeBeforeEruption = m_eruptionCooldown + m_distribution( m_generator ) * m_eruptionRandomCooldown;
			StartEruption();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::StartEruption()
	{
// 		m_state = COLLAPSING;
// 		m_eruptionTime = 0.f;
// 		m_particleSystem->SetEnabled( GetScene().IsServer() == false );
// 		m_material->SetColor( m_baseColor );
// 		m_particleSystem->SetSpeedParticles( 1.f );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::UpdateStateMachine( const float _delta )
	{
// 		SCOPED_PROFILE( state );
// 
// 		if ( m_state != NONE )
// 		{
// 			m_eruptionTime += _delta;
// 
// 			switch ( m_state )
// 			{
// 			case COLLAPSING:
// 			{
// 				const float scale = m_eruptionTime / m_warmingTime; // between 0-1
// 
// 				// Color
// 				Color color = m_baseColor;
// 				color[ 3 ] *= 1.f - ( 1.f - m_collapseAlpha ) * scale; // alpha between m_collapseAlpha / 1
// 				m_material->SetColor( color );
// 				m_particleSystem->SetParticlesColor( m_baseColor );
// 
// 				// Particles
// 				m_particleSystem->SetParticlesPerSecond( int( scale * m_warmingMaxParticles ) );
// 
// 				if ( m_eruptionTime > m_warmingTime )
// 				{
// 					m_eruptionTime -= m_warmingTime;
// 					m_state = WAITING;
// 				}
// 			} break;
// 			case WAITING:
// 			{
// 				const float scale = m_eruptionTime / m_waitingTime; // between 0-1
// 
// 				// Particles
// 				m_particleSystem->SetParticlesPerSecond( int( scale * ( m_waitingMaxParticles - m_warmingMaxParticles ) ) );
// 
// 				if ( m_eruptionTime > m_waitingTime )
// 				{
// 					m_eruptionTime -= m_waitingTime;
// 					m_state = EXPODING;
// 				}
// 			} break;
// 			case EXPODING:
// 			{
// 				const float scale = std::clamp( m_eruptionTime / m_explosionFadeinTime, 0.f, 1.f );
// 
// 				// particles
// 				m_particleSystem->SetParticlesPerSecond( int( m_explosionMaxParticles ) );
// 				m_particleSystem->SetSpeedParticles( m_explosionParticlesSpeed );
// 
// 				// scale
// 				m_gameobject->GetTransform().SetScale( btVector3( scale, 1.f, scale ) );
// 
// 				// color
// 				m_material->SetColor( m_explositonColor );
// 				m_particleSystem->SetParticlesColor( m_explositonColor );
// 
// 				// damage
// 				for ( Gameobject* player : m_players->GetPlayers() )
// 				{
// 					Health* health = player->GetComponent<Health>();
// 					SolarPanel* panel = player->GetComponent<SolarPanel>();
// 					if ( health != nullptr && panel != nullptr && panel->IsInsideSunlight() )
// 					{
// 						const float damage = _delta * m_eruptionDamagePerSecond;
// 						if ( !health->TryRemoveHealth( damage ) )
// 						{
// 							health->TryRemoveHealth( health->GetHealth() );
// 						}
// 					}
// 				}
// 
// 				if ( m_eruptionTime > m_explosionTime )
// 				{
// 					m_eruptionTime -= m_explosionTime;
// 					m_state = BACK_TO_NORMAL;
// 				}
// 			} break;
// 			case BACK_TO_NORMAL:
// 			{
// 				const float scale = m_eruptionTime / m_backToNormalTime;
// 
// 				m_material->SetColor( ( 1.f - scale ) * m_explositonColor.ToGLM() + scale * m_baseColor.ToGLM() );
// 				m_particleSystem->SetParticlesPerSecond( int( ( 1.f - scale ) * m_explosionMaxParticles ) );
// 
// 				if ( m_eruptionTime > m_backToNormalTime )
// 				{
// 					m_eruptionTime -= m_backToNormalTime;
// 					m_state = NONE;
// 
// 					m_particleSystem->SetEnabled( false );
// 					m_material->SetColor( m_baseColor );
// 				}
// 			} break;
// 			default:
// 			{
// 				m_state = NONE;
// 			} break;
// 			}
// 		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::OnGui()
	{
// 		ImGui::PushItemWidth( 200.f );
// 		{
// 			// State machine
// 			ImGui::DragFloat( "eruption cooldown", &m_eruptionCooldown );
// 			ImGui::DragFloat( "eruption random cooldown", &m_eruptionRandomCooldown );
// 			ImGui::DragFloat( "eruption damage per second", &m_eruptionDamagePerSecond, 1.f, 0.f, 100.f );
// 			ImGui::Spacing();
// 
// 			ImGui::DragFloat( "eruption time", &m_eruptionTime );
// 			ImGui::DragFloat( "warming time", &m_warmingTime );
// 			ImGui::DragFloat( "warming max particles", &m_warmingMaxParticles );
// 			ImGui::DragFloat( "waiting time", &m_waitingTime );
// 			ImGui::DragFloat( "waiting max particles", &m_waitingMaxParticles );
// 			ImGui::DragFloat( "explosion time", &m_explosionTime );
// 			ImGui::DragFloat( "explosion fade-in time", &m_explosionFadeinTime );
// 			ImGui::DragFloat( "explosion max particles", &m_explosionMaxParticles );
// 			ImGui::DragFloat( "explosion particles speed", &m_explosionParticlesSpeed );
// 			ImGui::DragFloat( "back to normal time", &m_backToNormalTime );
// 			ImGui::Spacing();
// 		} ImGui::PopItemWidth();
// 
// 		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
// 		{
// 			// Colors
// 			ImGui::Spacing();
// 			if ( ImGui::ColorEdit4( "base ", m_baseColor.Data(), ImGui::fanColorEditFlags ) )
// 			{
// 				m_material->SetColor( m_baseColor );
// 			}
// 			ImGui::ColorEdit4( "explositon ", m_explositonColor.Data(), ImGui::fanColorEditFlags );
// 
// 			// Collapse alpha
// 			ImGui::DragFloat( "collapse alpha", &m_collapseAlpha, 0.05f, 0.f, 1.f );
// 
// 			// Test
// 			ImGui::Spacing();
// 			if ( ImGui::Button( "Eruption" ) ) { StartEruption(); }
// 
// 		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SolarEruption::Load( const Json& _json )
	{


		// cooldown
		Serializable::LoadFloat( _json, "eruption_cooldown", m_eruptionCooldown );
		Serializable::LoadFloat( _json, "eruption_random_ooldown", m_eruptionRandomCooldown );
		Serializable::LoadFloat( _json, "eruption_damage_per_second", m_eruptionDamagePerSecond );

		// state machine		
		Serializable::LoadFloat( _json, "eruption time", m_eruptionTime );
		Serializable::LoadFloat( _json, "warming time", m_warmingTime );
		Serializable::LoadFloat( _json, "warming max particles", m_warmingMaxParticles );
		Serializable::LoadFloat( _json, "waiting time", m_waitingTime );
		Serializable::LoadFloat( _json, "waiting max particles", m_waitingMaxParticles );
		Serializable::LoadFloat( _json, "explosion time", m_explosionTime );
		Serializable::LoadFloat( _json, "explosion fade-in time", m_explosionFadeinTime );
		Serializable::LoadFloat( _json, "explosion max particles", m_explosionMaxParticles );
		Serializable::LoadFloat( _json, "explosion particles speed", m_explosionParticlesSpeed );

		// colors
		Serializable::LoadColor( _json, "base ", m_baseColor );
		Serializable::LoadColor( _json, "explositon ", m_explositonColor );
		Serializable::LoadFloat( _json, "collapse_alpha", m_collapseAlpha );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SolarEruption::Save( Json& _json ) const
	{


		// cooldown
		Serializable::SaveFloat( _json, "eruption_cooldown", m_eruptionCooldown );
		Serializable::SaveFloat( _json, "eruption_random_ooldown", m_eruptionRandomCooldown );
		Serializable::SaveFloat( _json, "eruption_damage_per_second", m_eruptionDamagePerSecond );

		// State machine		
		Serializable::SaveFloat( _json, "eruption time", m_eruptionTime );
		Serializable::SaveFloat( _json, "warming time", m_warmingTime );
		Serializable::SaveFloat( _json, "warming max particles", m_warmingMaxParticles );
		Serializable::SaveFloat( _json, "waiting time", m_waitingTime );
		Serializable::SaveFloat( _json, "waiting max particles", m_waitingMaxParticles );
		Serializable::SaveFloat( _json, "explosion time", m_explosionTime );
		Serializable::SaveFloat( _json, "explosion fade-in time", m_explosionFadeinTime );
		Serializable::SaveFloat( _json, "explosion max particles", m_explosionMaxParticles );
		Serializable::SaveFloat( _json, "explosion particles speed", m_explosionParticlesSpeed );

		// Colors
		Serializable::SaveColor( _json, "base ", m_baseColor );
		Serializable::SaveColor( _json, "explositon ", m_explositonColor );
		Serializable::SaveFloat( _json, "collapse_alpha", m_collapseAlpha );
		return true;
	}
}