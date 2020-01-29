#include "scene/actors/fanParticleSystem.hpp"

#include "scene/components/fanTransform.hpp"
#include "core/imgui/fanModals.hpp"

namespace fan {
	REGISTER_TYPE_INFO( ParticleSystem, TypeInfo::Flags::EDITOR_COMPONENT, "" )
		
	//================================================================================================================================
	//================================================================================================================================
	void ParticleSystem::Start() {  }

	//================================================================================================================================
	//================================================================================================================================
	void ParticleSystem::OnAttach() {
		Actor::OnAttach();

		m_ecsManager = &m_gameobject->GetScene().GetEcsManager();
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

		const Transform & transform = (*m_origin) != nullptr ? m_origin->GetTransform() : m_gameobject->GetTransform();

		const btVector3 origin =transform.GetPosition();
		btVector3 transformedOffset = transform.TransformDirection( m_offset );

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

		Serializable::LoadInt(   _json, "particles_per_second", m_particlesPerSecond );
		Serializable::LoadFloat( _json, "speed", m_speed);
		Serializable::LoadFloat( _json, "duration", m_duration );
		Serializable::LoadVec3(  _json, "offset", m_offset );
		Serializable::LoadGameobjectPtr(  _json, "origin", m_origin );
		Serializable::LoadColor( _json, "color", m_color );
		Serializable::LoadBool( _json, "sunlight_occlusion_active", m_sunlightParticleOcclusionActive);

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool ParticleSystem::Save( Json & _json ) const {
		Actor::Save( _json );

		Serializable::SaveInt( _json, "particles_per_second", m_particlesPerSecond );
		Serializable::SaveFloat( _json, "speed", m_speed );
		Serializable::SaveFloat( _json, "duration", m_duration );
		Serializable::SaveVec3( _json, "offset", m_offset );
		Serializable::SaveGameobjectPtr(  _json, "origin", m_origin );
		Serializable::SaveColor( _json, "color", m_color );
		Serializable::SaveBool( _json, "sunlight_occlusion_active", m_sunlightParticleOcclusionActive);

		return true;
	}
}