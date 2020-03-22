#include "game/components/fanPlayerInput.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanMouse.hpp"
#include "core/input/fanJoystick.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/components/fanTransform.hpp"
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
	void PlayerInput::Init( Component& _component )
	{
		PlayerInput& playerInput = static_cast<PlayerInput&>( _component );

		playerInput.directionBuffer.resize( 8, glm::vec2( 0 ) );
		playerInput.direction = btVector3( 0, 0, 1.f );
		playerInput.inputType = KEYBOARD_MOUSE;
		playerInput.joystickID = -1;
		playerInput.directionCutTreshold = 0.25f;
		playerInput.isReplicated = false;
		playerInput.inputData;
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::OnGui( Component& _component )
	{
		PlayerInput& playerInput = static_cast<PlayerInput&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			// Input type
			int type = playerInput.inputType;
			if( ImGui::Combo( "input type", &type, "keyboard+mouse\0joystick\0" ) )
			{
				playerInput.inputType = InputType( type );
			}
			ImGui::Checkbox( "replicated", &playerInput.isReplicated );

			ImGui::DragFloat( "fire", &playerInput.inputData.fire );

			if( playerInput.inputType == JOYSTICK )
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
		Serializable::LoadBool( _json, "replicated", playerInput.isReplicated );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::Save( const Component& _component, Json& _json )
	{
		const PlayerInput& playerInput = static_cast<const PlayerInput&>( _component );

		Serializable::SaveInt( _json, "direction_buffer_size", (int)playerInput.directionBuffer.size() );
		Serializable::SaveFloat( _json, "direction_cut_treshold", playerInput.directionCutTreshold );
		Serializable::SaveBool( _json, "replicated", playerInput.isReplicated );
	}

	//================================================================================================================================
	//================================================================================================================================
	float PlayerInput::GetInputLeft()
	{
		switch ( inputType )
		{
		case fan::PlayerInput::KEYBOARD_MOUSE:
			return Input::Get().Manager().GetAxis( "game_left", joystickID );
		case fan::PlayerInput::JOYSTICK:
		{
			float rawInput = Input::Get().Manager().GetAxis( "gamejs_axis_left", joystickID );
			return std::abs( rawInput ) > 0.15f ? rawInput : 0.f;
		}
		default:
			return 0.f;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::RefreshInput()
	{
		if ( !isReplicated )
		{
			inputData.direction = GetInputDirection();
			inputData.left = GetInputLeft();
			inputData.forward = GetInputForward();
			inputData.boost = GetInputBoost();
			inputData.fire = GetInputFire();
			inputData.stop = GetInputStop();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	float PlayerInput::GetInputForward()
	{
		switch ( inputType )
		{
		case fan::PlayerInput::KEYBOARD_MOUSE:
			return Input::Get().Manager().GetAxis( "game_forward" );
		case fan::PlayerInput::JOYSTICK:
		{
			const float x = Input::Get().Manager().GetAxis( "gamejs_x_axis_direction" );
			const float y = Input::Get().Manager().GetAxis( "gamejs_y_axis_direction", joystickID );
			float forward = btVector3( x, 0.f, y ).length();
			return forward > 0.2f ? forward : 0.f;
		} default:
			return 0.f;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	float PlayerInput::GetInputBoost()
	{
		switch ( inputType )
		{
		case fan::PlayerInput::KEYBOARD_MOUSE:
			return Input::Get().Manager().GetAxis( "game_boost" );
		case fan::PlayerInput::JOYSTICK:
			return Input::Get().Manager().GetAxis( "gamejs_axis_boost", joystickID );
		default:
			return 0.f;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	float PlayerInput::GetInputFire()
	{
		switch ( inputType )
		{
		case fan::PlayerInput::KEYBOARD_MOUSE:
			return Input::Get().Manager().GetAxis( "game_fire" );
		case fan::PlayerInput::JOYSTICK:
			return Input::Get().Manager().GetAxis( "gamejs_axis_fire", joystickID );
		default:
			return 0.f;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PlayerInput::GetInputStop()
	{
		switch ( inputType )
		{
		case fan::PlayerInput::KEYBOARD_MOUSE:
			return Input::Get().Manager().GetAxis( "game_axis_stop" ) > 0.f;
		case fan::PlayerInput::JOYSTICK:
			return Input::Get().Manager().GetAxis( "gamejs_axis_stop", joystickID ) > 0.f;
		default:
			return false;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 PlayerInput::GetInputDirection()
	{
// 		switch ( m_inputType )
// 		{
// 
// 		case fan::PlayerInput::KEYBOARD_MOUSE:
// 		{
// 			// Get mouse world pos @hack
// 			SceneNode& node = m_gameobject->GetScene().GetMainCamera();
// 			EcsWorld& world = node.scene->GetWorld();
// 			Camera2& camera = world.GetComponent<Camera2>( world.GetEntityID( node.entityHandle ) );
// 			Transform2& cameraTransform = world.GetComponent<Transform2>( world.GetEntityID( node.entityHandle ) );
// 			btVector3 mouseWorldPos = camera.ScreenPosToRay( cameraTransform, Mouse::Get().GetScreenSpacePosition() ).origin;
// 			mouseWorldPos.setY( 0 );
// 
// 			// Get mouse direction
// 			Transform& transform = m_gameobject->GetTransform();
// 			btVector3 mouseDir = mouseWorldPos - transform.GetPosition();
// 			mouseDir.normalize();
// 			return mouseDir;
// 		}
// 		case fan::PlayerInput::JOYSTICK:
// 		{
// 			glm::vec2 average = GetDirectionAverage();
// 
// 			btVector3 dir = btVector3( average.x, 0.f, average.y );
// 
// 			if ( dir.length() > m_directionCutTreshold ) { m_direction = dir; }
// 
// 			return m_direction;
// 		}
// 		default:
			return btVector3::Zero();
//		}
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::vec2 PlayerInput::GetDirectionAverage()
	{
		const size_t index = Input::Get().FrameCount() % directionBuffer.size();
		const float x = Input::Get().Manager().GetAxis( "gamejs_x_axis_direction", joystickID );
		const float y = Input::Get().Manager().GetAxis( "gamejs_y_axis_direction", joystickID );
		directionBuffer[ index ] = glm::vec2( x, y );

		glm::vec2 average( 0.f, 0.f );
		for ( int dirIndex = 0; dirIndex < directionBuffer.size(); dirIndex++ )
		{
			average += directionBuffer[ dirIndex ];
		}
		average /= float( directionBuffer.size() );

		return average;
	}
}