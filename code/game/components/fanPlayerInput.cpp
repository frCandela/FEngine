#include "game/components/fanPlayerInput.hpp"

#include "core/fanSerializable.hpp"
#include "core/input/fanJoystick.hpp"
#include "ecs/fanEcsWorld.hpp"

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
		_info.load = &PlayerInput::Load;
		_info.save = &PlayerInput::Save;
		_info.editorPath = "game/player/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::Init( EcsWorld& _world, Component& _component )
	{
		PlayerInput& playerInput = static_cast<PlayerInput&>( _component );

		playerInput.directionBuffer.resize( 8, glm::vec2( 0 ) );
		playerInput.direction = btVector3( 0, 0, 1.f );
		playerInput.type = KEYBOARD_MOUSE;
		playerInput.joystickID = -1;
		playerInput.directionCutTreshold = 0.25f;
		playerInput.inputData;
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
	{
		PlayerInput& playerInput = static_cast<PlayerInput&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			// Input type
			int type = playerInput.type;
			if( ImGui::Combo( "input type", &type, "keyboard+mouse\0joystick\0" ) )
			{
				playerInput.type = InputType( type );
			}

			ImGui::DragFloat( "fire", &playerInput.inputData.fire );

			if( playerInput.type == JOYSTICK )
			{
				ImGui::SliderInt( "joystick ID", &playerInput.joystickID, 0, Joystick::NUM_JOYSTICK - 1 );
			}

			// Direction buffer size
			int sizeBuffer = (int)playerInput.directionBuffer.size();
			if( ImGui::SliderInt( "direction buffer size", &sizeBuffer, 1, 64 ) )
			{
				playerInput.directionBuffer.resize( sizeBuffer, glm::vec2( 0 ) );
			}

			// direction cut threshold
			ImGui::DragFloat( "direction cut threshold", &playerInput.directionCutTreshold, 0.01f, 0.f, 1.f );

		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::Load( Component& _component, const Json& _json )
	{
		PlayerInput& playerInput = static_cast<PlayerInput&>( _component );

		int tmp;
		if( Serializable::LoadInt( _json, "direction_buffer_size", tmp ) )
		{
			playerInput.directionBuffer.resize( tmp, glm::vec3( 0.f ) );
		}
		Serializable::LoadFloat( _json, "direction_cut_treshold", playerInput.directionCutTreshold );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::Save( const Component& _component, Json& _json )
	{
		const PlayerInput& playerInput = static_cast<const PlayerInput&>( _component );

		Serializable::SaveInt( _json, "direction_buffer_size", (int)playerInput.directionBuffer.size() );
		Serializable::SaveFloat( _json, "direction_cut_treshold", playerInput.directionCutTreshold );
	}
}