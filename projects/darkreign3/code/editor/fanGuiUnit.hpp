#pragma once

#include "game/components/fanUnit.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiUnit
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::StickMan16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiUnit::OnGui;
            info.mEditorName = "Unit";
            info.mEditorPath = "game/";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
        {
            Unit& unit = static_cast<Unit&>( _component );
            ImGui::PushID( "Unit" );
            ImGui::DragFixed( "attack range", &unit.mAttackRange );
            ImGui::PushReadOnly();
            ImGui::DragInt( "state", (int*)&unit.mState );
            ImGui::PopReadOnly();
            ImGui::PopID();
        }
    };
}