#pragma once

#include "game/components/fanPlayerInput.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    struct GuiPlayerInput
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Input16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiPlayerInput::OnGui;
            info.mEditorPath = "game/player/";
            info.mEditorName = "player_input";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            PlayerInput& playerInput = static_cast<PlayerInput&>( _component );

            ImGui::PushReadOnly();
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                ImGui::DragFloat2( "orientation", &playerInput.mOrientation[0] );
                ImGui::DragFloat( "left", &playerInput.mLeft );
                ImGui::DragFloat( "forward", &playerInput.mForward );
                ImGui::DragFloat( "boost", &playerInput.mBoost );
                ImGui::DragFloat( "fire", &playerInput.mFire );
            }
            ImGui::PopItemWidth();
            ImGui::PopReadOnly();
        }
    };
}