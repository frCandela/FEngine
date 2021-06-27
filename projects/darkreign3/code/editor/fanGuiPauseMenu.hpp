#pragma once

#include "game/singletons/fanPauseMenu.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"
#include "editor/gui/fanGuiResourcePtr.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiPauseMenu
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon       = ImGui::Pause16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiPauseMenu::OnGui;
            info.mEditorName = "Pause Menu";
            return info;
        }

        static void OnGui( EcsWorld& _world, EcsSingleton& _singleton )
        {
            PauseMenu& pauseMenu = static_cast<PauseMenu&>( _singleton );
            ImGui::FanResourcePtr<Prefab>( "header", pauseMenu.mHeader );
            ImGui::FanResourcePtr<Prefab>( "background", pauseMenu.mBackground );

            if( ImGui::Button( "build pause menu" ) )
            {
                PauseMenu::Instantiate( _world );
            }
            ImGui::PushReadOnly();
            int handleCpy = (int)pauseMenu.mHandle;
            ImGui::DragInt( "handle", &handleCpy );
            ImGui::PopReadOnly();
        }
    };
}