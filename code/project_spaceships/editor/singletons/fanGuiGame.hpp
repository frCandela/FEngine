#pragma once

#include "project_spaceships/game/singletons/fanGame.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiGame
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon  = ImGui::Joystick16;
            info.mGroup = EngineGroups::Game;
            info.onGui  = &GuiGame::OnGui;
            info.mEditorName  = "game";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            Game& gameData = static_cast<Game&>( _component );
            ImGui::FanPrefab( "spaceship", gameData.mSpaceshipPrefab );
        }
    };
}