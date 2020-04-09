#include "game/components/fanPlayerInput.hpp"

#include "core/fanSerializable.hpp"
#include "core/input/fanJoystick.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "editor/fanModals.hpp"	

namespace fan
{
	REGISTER_COMPONENT( PlayerInput, "player_input" );

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::JOYSTICK16;
		_info.onGui = &PlayerInput::OnGui;
		_info.init = &PlayerInput::Init;
		_info.save = &PlayerInput::Save;
		_info.load = &PlayerInput::Load;
		_info.editorPath = "game/player/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::Init( EcsWorld& _world, Component& _component )
	{
		PlayerInput& playerInput = static_cast<PlayerInput&>( _component );
		playerInput.orientation = btVector3(0,0,1);
		playerInput.left = 0.f;
		playerInput.forward = 0.f;
		playerInput.boost = 0.f;
		playerInput.fire = 0.f;
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
	{
		PlayerInput& playerInput = static_cast<PlayerInput&>( _component );

		ImGui::PushReadOnly();
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::DragFloat2( "orientation", &playerInput.orientation[0] );
			ImGui::DragFloat( "left", &playerInput.left );
			ImGui::DragFloat( "forward", &playerInput.forward );
			ImGui::DragFloat( "boost", &playerInput.boost );
			ImGui::DragFloat( "fire", &playerInput.fire );
		} ImGui::PopItemWidth();
		ImGui::PopReadOnly();
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::Save( const Component& _component, Json& _json ){}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::Load( Component& _component, const Json& _json ){}
}