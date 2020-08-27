#include "game/components/fanPlayerInput.hpp"

#include "core/fanSerializable.hpp"
#include "core/input/fanJoystick.hpp"
#include "ecs/fanEcsWorld.hpp"
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
		_info.save         = &PlayerInput::Save;
		_info.load         = &PlayerInput::Load;
		_info.rollbackLoad = &PlayerInput::RollbackLoad;
		_info.rollbackSave = &PlayerInput::RollbackSave;
		_info.mEditorPath  = "game/player/";
		_info.mName        = "player_input";
		_info.mFlags |= EcsComponentInfo::RollbackNoOverwrite;
	}

	//========================================================================================================
	//========================================================================================================
	void PlayerInput::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		PlayerInput& playerInput = static_cast<PlayerInput&>( _component );
		playerInput.mOrientation = btVector3( 0, 0, 1);
		playerInput.mLeft        = 0.f;
		playerInput.mForward     = 0.f;
		playerInput.mBoost       = 0.f;
		playerInput.mFire        = 0.f;
	}

	//========================================================================================================
	//========================================================================================================
	void PlayerInput::RollbackSave( const EcsComponent& _component, sf::Packet& _packet )
	{
		const PlayerInput& playerInput = static_cast<const PlayerInput&>( _component );

		_packet << playerInput.mOrientation[0] << playerInput.mOrientation[2];
		_packet << playerInput.mLeft;
		_packet << playerInput.mForward;
		_packet << playerInput.mBoost;
		_packet << playerInput.mFire;
	}

	//========================================================================================================
	//========================================================================================================
	void PlayerInput::RollbackLoad( EcsComponent& _component, sf::Packet& _packet )
	{
		PlayerInput& playerInput = static_cast<PlayerInput&>( _component );

		_packet >> playerInput.mOrientation[0] >> playerInput.mOrientation[2];
		_packet >> playerInput.mLeft;
		_packet >> playerInput.mForward;
		_packet >> playerInput.mBoost;
		_packet >> playerInput.mFire;
	}

	//========================================================================================================
	//========================================================================================================
	void PlayerInput::Save( const EcsComponent& /*_component*/, Json& /*_json*/ ){}

	//========================================================================================================
	//========================================================================================================
	void PlayerInput::Load( EcsComponent& /*_component*/, const Json& /*_json*/ ){}

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