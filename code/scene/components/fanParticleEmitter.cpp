#include "scene/components/fanParticleEmitter.hpp"
#include "render/fanRenderSerializable.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ParticleEmitter::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::PARTICLES16;
		_info.group = EngineGroups::Scene;
		_info.onGui = &ParticleEmitter::OnGui;
		_info.load = &ParticleEmitter::Load;
		_info.save = &ParticleEmitter::Save;
		_info.editorPath = "/";
		_info.name = "particle emitter";
	}

	//========================================================================================================
	//========================================================================================================
	void ParticleEmitter::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ParticleEmitter& emitter = static_cast<ParticleEmitter&>( _component );
		emitter.mEnabled            = true;
		emitter.mParticlesPerSecond = 100.f;
		emitter.mSpeed              = 1.f;
		emitter.mDuration           = 2.f;
		emitter.mColor              = Color::Magenta;
		emitter.mTimeAccumulator    = 0.f;
		emitter.mTagsSignature      = 0;
	}

	//========================================================================================================
	//========================================================================================================
	void ParticleEmitter::Save( const EcsComponent& _component, Json& _json )
	{
		const ParticleEmitter& emitter = static_cast<const ParticleEmitter&>( _component );

		Serializable::SaveBool( _json, "enabled", emitter.mEnabled );
		Serializable::SaveFloat( _json, "particles_per_second", emitter.mParticlesPerSecond );
		Serializable::SaveFloat( _json, "speed", emitter.mSpeed );
		Serializable::SaveFloat( _json, "duration", emitter.mDuration );
		Serializable::SaveColor( _json, "color", emitter.mColor );
	}

	//========================================================================================================
	//========================================================================================================
	void ParticleEmitter::Load( EcsComponent& _component, const Json& _json )
	{
		ParticleEmitter& emitter = static_cast<ParticleEmitter&>( _component );

		Serializable::LoadBool( _json, "enabled", emitter.mEnabled );
		Serializable::LoadFloat( _json, "particles_per_second", emitter.mParticlesPerSecond );
		Serializable::LoadFloat( _json, "speed", emitter.mSpeed );
		Serializable::LoadFloat( _json, "duration", emitter.mDuration );
		Serializable::LoadColor( _json, "color", emitter.mColor );
	}

    //========================================================================================================
    //========================================================================================================
    void ParticleEmitter::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        ParticleEmitter& emitter = static_cast<ParticleEmitter&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            ImGui::Checkbox( "enabled", &emitter.mEnabled );
            ImGui::DragFloat( "particles per second", &emitter.mParticlesPerSecond, 1.f, 0.f, 10000.f );
            ImGui::DragFloat( "speed##ParticleEmitter", &emitter.mSpeed, 0.01f );
            ImGui::DragFloat( "duration##ParticleEmitter", &emitter.mDuration, 0.01f );
            ImGui::ColorEdit4( "color##ParticleEmitter", emitter.mColor.Data(), ImGui::fanColorEditFlags );
        }
        ImGui::PopItemWidth();
    }
}