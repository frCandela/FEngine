#pragma once

#include "scene/components/fanParticleEmitter.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ParticleEmitter::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Particles16;
		_info.mGroup      = EngineGroups::Scene;
		_info.onGui       = &ParticleEmitter::OnGui;
		_info.mEditorPath = "/";
		_info.mName       = "particle emitter";
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