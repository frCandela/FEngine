#pragma once

#include "game/units/fanJudas.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiJudas
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::StickMan16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiJudas::OnGui;
            info.mEditorName = "Judas";
            info.mEditorPath = "units/";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
        {
            Judas& judas = static_cast<Judas&>( _component );
            ImGui::PushID("Judas");
            ImGui::FanResourcePtr<Animation>( "anim idle", judas.mAnimIdle );
            ImGui::FanResourcePtr<Animation>( "anim walk", judas.mAnimWalk );
            ImGui::FanResourcePtr<Animation>( "anim run", judas.mAnimRun );
            ImGui::FanResourcePtr<Animation>( "anim fire", judas.mAnimFire );
            ImGui::PopID();
        }
    };
}