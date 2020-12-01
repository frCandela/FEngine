#pragma once

#include "project_spaceships/components/fanPlayerController.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiPlayerController
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Joystick16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiPlayerController::OnGui;
            info.mEditorPath = "game/player/";
            info.mEditorName = "player_controller";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& /*_component*/ )
        {
//		PlayerController& playerController = static_cast<PlayerController&>( _component );

            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
// 			// Input type
// 			int type = playerInput.type;
// 			if( ImGui::Combo( "input type", &type, "keyboard+mouse\0joystick\0" ) )
// 			{
// 				playerInput.type = InputType( type );
// 			}
//
// 			ImGui::DragFloat( "fire", &playerInput.inputData.fire );
//
// 			if( playerInput.type == JOYSTICK )
// 			{
// 				ImGui::SliderInt( "joystick ID", &playerInput.joystickID, 0, Joystick::NUM_JOYSTICK - 1 );
// 			}
//
// 			// Direction buffer size
// 			int sizeBuffer = (int)playerInput.directionBuffer.size();
// 			if( ImGui::SliderInt( "direction buffer size", &sizeBuffer, 1, 64 ) )
// 			{
// 				playerInput.directionBuffer.resize( sizeBuffer, glm::vec2( 0 ) );
// 			}
//
// 			// direction cut threshold
// 			ImGui::DragFloat( "direction cut threshold", &playerInput.directionCutTreshold, 0.01f, 0.f, 1.f );

            }
            ImGui::PopItemWidth();
        }
    };
}