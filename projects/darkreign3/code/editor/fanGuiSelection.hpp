#pragma once

#include "game/singletons/fanSelection.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"
#include "editor/gui/fanGuiResourcePtr.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiSelection
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon       = ImGui::None16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiSelection::OnGui;
            info.mEditorName = "Selection";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsSingleton& _singleton )
        {
            Selection& selection = static_cast<Selection&>( _singleton );
            ImGui::FanResourcePtr<Prefab>( "selection frame", selection.mSelectionFramePrefab );
            ImGui::FanResourcePtr<Prefab>( "move to fx", selection.mMoveToFxPrefab );
            ImGui::FanResourcePtr<Prefab>( "health bar", selection.mHealthBarPrefab );
        }
    };
}