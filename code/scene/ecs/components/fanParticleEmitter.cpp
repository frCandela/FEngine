#include "scene/ecs/components/fanParticleEmitter.hpp"

#include "scene/ecs/fanEntityWorld.hpp"
#include "render/fanRenderSerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( ParticleEmitter, "particle emitter" );

	//================================================================================================================================
	//================================================================================================================================
	void ParticleEmitter::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::PARTICLES16;
		_info.onGui = &ParticleEmitter::OnGui;
		_info.clear = &ParticleEmitter::Clear;
		_info.load = &ParticleEmitter::Load;
		_info.save = &ParticleEmitter::Save;
		_info.editorPath = "";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ParticleEmitter::Clear( ecComponent& _emitter )
	{
		ParticleEmitter& emitter = static_cast<ParticleEmitter&>( _emitter );

		emitter.particlesPerSecond = 1;
		emitter.speed = 1.f;
		emitter.duration = 2.f;
		emitter.offset = btVector3::Zero();
		emitter.color = Color::White;
		emitter.timeAccumulator = 0.f;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ParticleEmitter::OnGui( ecComponent& _emitter )
	{
		ParticleEmitter& emitter = static_cast<ParticleEmitter&>( _emitter );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::DragInt( "particles per second", &emitter.particlesPerSecond, 1, 0 );
			ImGui::DragFloat( "speed", &emitter.speed, 0.01f );
			ImGui::DragFloat( "duration", &emitter.duration, 0.01f );
			ImGui::DragFloat3( "offset", &emitter.offset[0], 0.01f );
			ImGui::ColorEdit4( "color##pecol", emitter.color.Data(), ImGui::fanColorEditFlags );

		}
		ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ParticleEmitter::Save( const ecComponent& _emitter, Json& _json )
	{
		const ParticleEmitter& emitter = static_cast<const ParticleEmitter&>( _emitter );

		Serializable::SaveInt( _json, "particles_per_second", emitter.particlesPerSecond );
		Serializable::SaveFloat( _json, "speed", emitter.speed );
		Serializable::SaveFloat( _json, "duration", emitter.duration );
		Serializable::SaveVec3( _json, "offset", emitter.offset );
		Serializable::SaveColor( _json, "color", emitter.color );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ParticleEmitter::Load( ecComponent& _emitter, const Json& _json )
	{
		ParticleEmitter& emitter = static_cast<ParticleEmitter&>( _emitter );

		Serializable::LoadInt( _json, "particles_per_second", emitter.particlesPerSecond );
		Serializable::LoadFloat( _json, "speed", emitter.speed );
		Serializable::LoadFloat( _json, "duration", emitter.duration );
		Serializable::LoadVec3( _json, "offset", emitter.offset );
		Serializable::LoadColor( _json, "color", emitter.color );
	}
}

	//================================================================================================================================
	//================================================================================================================================
// 	void ParticleSystem::Update( const float _delta )
// 	{
// 		if ( m_particlesPerSecond <= 0.f )
// 		{
// 			return;
// 		}
// 
// 		m_timeAccumulator += _delta;
// 		float particleSpawnDelta = 1.f / m_particlesPerSecond;
// 
// 		const Transform& transform = ( *m_origin ) != nullptr ? m_origin->GetTransform() : m_gameobject->GetTransform();
// 
// 		const btVector3 origin = transform.GetPosition();
// 		btVector3 transformedOffset = transform.TransformDirection( m_offset );
// 
// 		while ( m_timeAccumulator > particleSpawnDelta )
// 		{
// 			m_timeAccumulator -= particleSpawnDelta;
// 
// 			ecsEntity entity = m_ecsManager->CreateEntity();
// 			m_ecsManager->AddComponent<ecsPosition>( entity );
// 			m_ecsManager->AddComponent<ecsRotation>( entity );
// 			m_ecsManager->AddComponent<ecsMovement>( entity );
// 			m_ecsManager->AddComponent<ecsParticle>( entity );
// 			if ( m_sunlightParticleOcclusionActive ) { m_ecsManager->AddComponent<ecsSunlightParticleOcclusion>( entity ); }
// 
// 			ecsPosition* position = m_ecsManager->FindComponentFromEntity<ecsPosition>( entity );
// 			ecsMovement* movement = m_ecsManager->FindComponentFromEntity<ecsMovement>( entity );
// 			ecsParticle* particle = m_ecsManager->FindComponentFromEntity<ecsParticle>( entity );
// 
// 
// 			// Spawn particles
// 
// 			movement->speed = btVector3( m_distribution( m_generator ), m_distribution( m_generator ), m_distribution( m_generator ) ) - btVector3( 0.5f, 0.5f, 0.5f );
// 			movement->speed.normalize();
// 			movement->speed *= m_speed;
// 			position->position = origin + transformedOffset;
// 			particle->durationLeft = m_duration;
// 			particle->color = m_color;
// 		}
// 	}