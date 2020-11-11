#include "game/components/fanPlayerInput.hpp"

#include "core/input/fanJoystick.hpp"
#include "editor/fanModals.hpp"	
#include "network/fanPacket.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void PlayerInput::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon        = ImGui::IconType::Input16;
		_info.mGroup       = EngineGroups::Game;
		_info.onGui        = &PlayerInput::OnGui;
		_info.mEditorPath  = "game/player/";
		_info.mName        = "player_input";
	}

    //========================================================================================================
    //========================================================================================================
    void PlayerInput::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
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
        } ImGui::PopItemWidth();
        ImGui::PopReadOnly();
    }
}