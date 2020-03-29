#include "scene/components/fanParticleEmitter.hpp"

#include "render/fanRenderSerializable.hpp"
#include "editor/fanModals.hpp"
#include "editor/fanImguiIcons.hpp"

namespace fan
{
	REGISTER_COMPONENT( ParticleEmitter, "particle_emitter" );

	//================================================================================================================================
	//================================================================================================================================
	void ParticleEmitter::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::PARTICLES16;
		_info.onGui = &ParticleEmitter::OnGui;
		_info.init = &ParticleEmitter::Init;
		_info.load = &ParticleEmitter::Load;
		_info.save = &ParticleEmitter::Save;
		_info.editorPath = "/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ParticleEmitter::Init( EcsWorld& _world, Component& _component )
	{
		ParticleEmitter& emitter = static_cast<ParticleEmitter&>( _component );

		emitter.particlesPerSecond = 100;
		emitter.speed = 1.f;
		emitter.duration = 2.f;
		emitter.offset = btVector3::Zero();
		emitter.color = Color::Magenta;
		emitter.timeAccumulator = 0.f;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ParticleEmitter::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
	{
		ParticleEmitter& emitter = static_cast<ParticleEmitter&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::DragInt( "particles per second", &emitter.particlesPerSecond, 1, 0 );
			ImGui::DragFloat( "speed##ParticleEmitter", &emitter.speed, 0.01f );
			ImGui::DragFloat( "duration##ParticleEmitter", &emitter.duration, 0.01f );
			ImGui::DragFloat3( "offset##ParticleEmitter", &emitter.offset[0], 0.01f );
			ImGui::ColorEdit4( "color##ParticleEmitter", emitter.color.Data(), ImGui::fanColorEditFlags );
		}
		ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ParticleEmitter::Save( const Component& _component, Json& _json )
	{
		const ParticleEmitter& emitter = static_cast<const ParticleEmitter&>( _component );

		Serializable::SaveInt( _json, "particles_per_second", emitter.particlesPerSecond );
		Serializable::SaveFloat( _json, "speed", emitter.speed );
		Serializable::SaveFloat( _json, "duration", emitter.duration );
		Serializable::SaveVec3( _json, "offset", emitter.offset );
		Serializable::SaveColor( _json, "color", emitter.color );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ParticleEmitter::Load( Component& _component, const Json& _json )
	{
		ParticleEmitter& emitter = static_cast<ParticleEmitter&>( _component );

		Serializable::LoadInt( _json, "particles_per_second", emitter.particlesPerSecond );
		Serializable::LoadFloat( _json, "speed", emitter.speed );
		Serializable::LoadFloat( _json, "duration", emitter.duration );
		Serializable::LoadVec3( _json, "offset", emitter.offset );
		Serializable::LoadColor( _json, "color", emitter.color );
	}
}