#include "fanGlobalIncludes.h"
#include "scene/actors/fanParticleSystem.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"
#include "renderer/fanRenderer.h"
#include "editor/fanModals.h"

namespace fan {
	REGISTER_TYPE_INFO( ParticleSystem, TypeInfo::Flags::EDITOR_VISIBLE )

	//================================================================================================================================
	//================================================================================================================================
	void ParticleSystem::Start() {}

	//================================================================================================================================
	//================================================================================================================================
	void ParticleSystem::OnAttach() {
		Actor::OnAttach();

		m_ecsManager = m_gameobject->GetScene()->GetEcsManager();
		m_distribution = std::uniform_real_distribution<float> ( 0.f, 1.f );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ParticleSystem::OnDetach() {
		Actor::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ParticleSystem::Update( const float _delta ) {
		if ( m_particlesPerSecond <= 0.f ) {
			return;
		}
		
		m_timeAccumulator += _delta;
		float particleSpawnDelta = 1.f / m_particlesPerSecond;

		const Transform * transform = (*m_origin) != nullptr ? m_origin->GetTransform() : m_gameobject->GetTransform();

		const btVector3 origin =transform->GetPosition();
		btVector3 transformedOffset = transform->TransformDirection( m_offset );

		while ( m_timeAccumulator > particleSpawnDelta ) {
			m_timeAccumulator -= particleSpawnDelta;

			ecsEntity entity = m_ecsManager->CreateEntity();
			m_ecsManager->AddComponent<ecsPosition>( entity );
			m_ecsManager->AddComponent<ecsRotation>( entity );
			m_ecsManager->AddComponent<ecsMovement>( entity );
			m_ecsManager->AddComponent<ecsParticle>( entity );
			if( m_sunlightParticleOcclusionActive ) { m_ecsManager->AddComponent<ecsSunlightParticleOcclusion>( entity );}

			ecsPosition* position = m_ecsManager->FindComponentFromEntity<ecsPosition>( entity );
			ecsMovement* movement = m_ecsManager->FindComponentFromEntity<ecsMovement>( entity );
			ecsParticle* particle = m_ecsManager->FindComponentFromEntity<ecsParticle>( entity );

			
			// Spawn particles
			
			movement->speed = btVector3( m_distribution( m_generator ), m_distribution( m_generator ), m_distribution( m_generator ) ) - btVector3( 0.5f, 0.5f, 0.5f );
			movement->speed.normalize();
			movement->speed *= m_speed;
			position->position = origin + transformedOffset;
			particle->durationLeft = m_duration;
			particle->color = m_color;
		}	
	}

	//================================================================================================================================
	//================================================================================================================================
	void ParticleSystem::OnGui() 
	{
		Actor::OnGui();

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() ); {

			ImGui::FanGameobject( "origin", &m_origin );
			ImGui::DragInt( "particles per second", &m_particlesPerSecond, 1, 0 );
			ImGui::DragFloat( "speed", &m_speed, 0.01f );
			ImGui::DragFloat( "duration", &m_duration, 0.01f );
			ImGui::DragFloat3( "offset", &m_offset[0], 0.01f );	
		    ImGui::ColorEdit4( "color", m_color.Data(), ImGui::fanColorEditFlags );
			ImGui::Checkbox("sun light occlusion", &m_sunlightParticleOcclusionActive);

		} ImGui::PopItemWidth();

	}

	//================================================================================================================================
	//================================================================================================================================
	bool ParticleSystem::Load( const Json & _json ) {
		Actor::Load( _json );

		LoadInt(   _json, "particles_per_second", m_particlesPerSecond );
		LoadFloat( _json, "speed", m_speed);
		LoadFloat( _json, "duration", m_duration );
		LoadVec3(  _json, "offset", m_offset );
		LoadGameobjectPtr(  _json, "origin", m_origin );
		LoadColor( _json, "color", m_color );
		LoadBool( _json, "sunlight_occlusion_active", m_sunlightParticleOcclusionActive);

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool ParticleSystem::Save( Json & _json ) const {
		Actor::Save( _json );

		SaveInt( _json, "particles_per_second", m_particlesPerSecond );
		SaveFloat( _json, "speed", m_speed );
		SaveFloat( _json, "duration", m_duration );
		SaveVec3( _json, "offset", m_offset );
		SaveGameobjectPtr(  _json, "origin", m_origin );
		SaveColor( _json, "color", m_color );
		SaveBool( _json, "sunlight_occlusion_active", m_sunlightParticleOcclusionActive);

		return true;
	}
}