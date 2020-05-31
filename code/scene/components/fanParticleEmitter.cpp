#include "scene/components/fanParticleEmitter.hpp"

#include "render/fanRenderSerializable.hpp"
#include "editor/fanModals.hpp"
#include "editor/fanImguiIcons.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void ParticleEmitter::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::PARTICLES16;
		_info.onGui = &ParticleEmitter::OnGui;
		_info.load = &ParticleEmitter::Load;
		_info.save = &ParticleEmitter::Save;
		_info.editorPath = "/";
		_info.name = "particle emitter";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ParticleEmitter::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ParticleEmitter& emitter = static_cast<ParticleEmitter&>( _component );
		emitter.enabled = true;
		emitter.particlesPerSecond = 100.f;
		emitter.speed = 1.f;
		emitter.duration = 2.f;
		emitter.color = Color::Magenta;
		emitter.timeAccumulator = 0.f;
		emitter.tagsSignature = 0;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ParticleEmitter::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ParticleEmitter& emitter = static_cast<ParticleEmitter&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::Checkbox( "enabled", &emitter.enabled );
			ImGui::DragFloat( "particles per second", &emitter.particlesPerSecond, 1.f, 0.f, 10000.f );
			ImGui::DragFloat( "speed##ParticleEmitter", &emitter.speed, 0.01f );
			ImGui::DragFloat( "duration##ParticleEmitter", &emitter.duration, 0.01f );
			ImGui::ColorEdit4( "color##ParticleEmitter", emitter.color.Data(), ImGui::fanColorEditFlags );
		}
		ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ParticleEmitter::Save( const EcsComponent& _component, Json& _json )
	{
		const ParticleEmitter& emitter = static_cast<const ParticleEmitter&>( _component );

		Serializable::SaveBool( _json, "enabled", emitter.enabled );
		Serializable::SaveFloat( _json, "particles_per_second", emitter.particlesPerSecond );
		Serializable::SaveFloat( _json, "speed", emitter.speed );
		Serializable::SaveFloat( _json, "duration", emitter.duration );
		Serializable::SaveColor( _json, "color", emitter.color );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ParticleEmitter::Load( EcsComponent& _component, const Json& _json )
	{
		ParticleEmitter& emitter = static_cast<ParticleEmitter&>( _component );

		Serializable::LoadBool( _json, "enabled", emitter.enabled );
		Serializable::LoadFloat( _json, "particles_per_second", emitter.particlesPerSecond );
		Serializable::LoadFloat( _json, "speed", emitter.speed );
		Serializable::LoadFloat( _json, "duration", emitter.duration );
		Serializable::LoadColor( _json, "color", emitter.color );
	}
}