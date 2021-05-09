#pragma once

#include "engine/components/fanParticleEmitter.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiParticleEmitter
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Particles16;
            info.mGroup      = EngineGroups::Scene;
            info.onGui       = &GuiParticleEmitter::OnGui;
            info.mEditorPath = "/";
            info.mEditorName = "particle emitter";

            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            ParticleEmitter& emitter = static_cast<ParticleEmitter&>( _component );

            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                ImGui::Checkbox( "enabled", &emitter.mEnabled );
                ImGui::DragFloat( "particles per second", &emitter.mParticlesPerSecond, 1.f, 0.f, 10000.f );
                ImGui::DragFloat( "speed##ParticleEmitter", &emitter.mSpeed, 0.01f );
                ImGui::DragFloat( "duration##ParticleEmitter", &emitter.mDuration, 0.01f );
                ImGui::ColorEdit4( "color##ParticleEmitter",
                                   emitter.mColor.Data(),
                                   ImGui::fanColorEditFlags );
            }
            ImGui::PopItemWidth();
        }
    };
}