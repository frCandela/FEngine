#include "fanGlobalIncludes.h"
#include "scene/actors/fanParticleSystem.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"

namespace fan {
	REGISTER_EDITOR_COMPONENT( ParticleSystem )
	REGISTER_TYPE_INFO( ParticleSystem )

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

		const btVector3 origin = m_gameobject->GetTransform()->GetPosition();

		btTransform transform( m_gameobject->GetTransform()->GetRotationQuat() );
		btVector3 transformedOffset = transform * m_offset;

		// Spawn particles
		while ( m_timeAccumulator > particleSpawnDelta ) {
			m_timeAccumulator -= particleSpawnDelta;

			ecsEntity entity = m_ecsManager->CreateEntity();
			m_ecsManager->AddComponent<ecsPosition>( entity );
			m_ecsManager->AddComponent<ecsRotation>( entity );
			m_ecsManager->AddComponent<ecsMovement>( entity );
			m_ecsManager->AddComponent<ecsParticle>( entity );

			ecsPosition* position = m_ecsManager->FindComponentFromEntity<ecsPosition>( entity );
			ecsMovement* movement = m_ecsManager->FindComponentFromEntity<ecsMovement>( entity );
			ecsParticle* particle = m_ecsManager->FindComponentFromEntity<ecsParticle>( entity );

			movement->speed = btVector3( m_distribution( m_generator ), m_distribution( m_generator ), m_distribution( m_generator ) ) - btVector3( 0.5f, 0.5f, 0.5f );
			movement->speed.normalize();
			movement->speed *= m_speed;
			position->position = origin + transformedOffset;
			particle->durationLeft = m_duration;
		}	
	}

	//================================================================================================================================
	//================================================================================================================================
	void ParticleSystem::OnGui() {
		Actor::OnGui();

		ImGui::DragInt( "particles per second", &m_particlesPerSecond, 1, 0 );
		ImGui::DragFloat( "speed", &m_speed, 0.01f );
		ImGui::DragFloat( "duration", &m_duration, 0.01f );
		ImGui::DragFloat3( "offset", &m_offset[0], 0.01f );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool ParticleSystem::Load( Json & _json ) {
		Actor::Load( _json );

		LoadInt(  _json, "particles_per_second", m_particlesPerSecond );
		LoadFloat(_json, "speed", m_speed);
		LoadFloat(_json, "duration", m_duration );
		LoadVec3( _json, "offset", m_offset );

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

		return true;
	}
}