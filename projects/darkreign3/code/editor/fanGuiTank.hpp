#pragma once

#include "game/units/fanTank.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiTank
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::StickMan16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiTank::OnGui;
            info.mEditorName = "Tank";
            info.mEditorPath = "units/";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
        {
            Tank& tank = static_cast<Tank&>( _component );
            ImGui::PushID( "Tabk" );
            ImGui::FanResourcePtr<Prefab>( "fire fx", tank.mFireFx );
            ImGui::FanResourcePtr<Prefab>( "death fx", tank.mDeathFx );
            ImGui::PopID();
        }
    };
}