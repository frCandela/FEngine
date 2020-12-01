#pragma once

#include "project_spaceships/game/components/fanSpaceshipUI.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiSpaceshipUI
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Spaceship16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiSpaceshipUI::OnGui;
            info.mEditorPath = "game/";
            info.mEditorName = "spaceship ui";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            SpaceshipUI& spaceshipUI = static_cast<SpaceshipUI&>( _component );

            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                ImGui::DragFloat2( "ui offset", &spaceshipUI.mUIOffset[0], 1.f );
                ImGui::FanComponent( "ui root transform", spaceshipUI.mUIRootTransform );
                ImGui::FanComponent( "health progress", spaceshipUI.mHealthProgress );
                ImGui::FanComponent( "energy progress", spaceshipUI.mEnergyProgress );
                ImGui::FanComponent( "signal progress", spaceshipUI.mSignalProgress );
                ImGui::FanComponent( "signal renderer", spaceshipUI.mSignalRenderer );
            }
            ImGui::PopItemWidth();
        }
    };
}