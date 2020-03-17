#include "game/components/fanPlayerInput.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanMouse.hpp"
#include "core/input/fanJoystick.hpp"
#include "scene/ecs/components/fanSceneNode.hpp"
#include "scene/ecs/components/fanCamera.hpp"
#include "scene/ecs/components/fanTransform.hpp"
#include "scene/ecs/fanEcsWorld.hpp"

namespace fan
{

		//================================================================================================================================
		//================================================================================================================================
		void PlayerInput::OnAttach()
	{
		

		m_directionBuffer.resize( 8, glm::vec2( 0 ) );
		m_direction = btVector3( 0, 0, 1.f );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::OnDetach()
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::SetInputType( const InputType _type )
	{
		if ( _type == m_inputType ) { return; }
		m_inputType = _type;
	}

	//================================================================================================================================
	//================================================================================================================================
	float PlayerInput::GetInputLeft()
	{
		switch ( m_inputType )
		{
		case fan::PlayerInput::KEYBOARD_MOUSE:
			return Input::Get().Manager().GetAxis( "game_left", m_joystickID );
		case fan::PlayerInput::JOYSTICK:
		{
			float rawInput = Input::Get().Manager().GetAxis( "gamejs_axis_left", m_joystickID );
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
		if ( !m_isReplicated )
		{
			m_inputData.direction = GetInputDirection();
			m_inputData.left = GetInputLeft();
			m_inputData.forward = GetInputForward();
			m_inputData.boost = GetInputBoost();
			m_inputData.fire = GetInputFire();
			m_inputData.stop = GetInputStop();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	float PlayerInput::GetInputForward()
	{
		switch ( m_inputType )
		{
		case fan::PlayerInput::KEYBOARD_MOUSE:
			return Input::Get().Manager().GetAxis( "game_forward" );
		case fan::PlayerInput::JOYSTICK:
		{
			const float x = Input::Get().Manager().GetAxis( "gamejs_x_axis_direction", m_joystickID );
			const float y = Input::Get().Manager().GetAxis( "gamejs_y_axis_direction", m_joystickID );
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
		switch ( m_inputType )
		{
		case fan::PlayerInput::KEYBOARD_MOUSE:
			return Input::Get().Manager().GetAxis( "game_boost", m_joystickID );
		case fan::PlayerInput::JOYSTICK:
			return Input::Get().Manager().GetAxis( "gamejs_axis_boost", m_joystickID );
		default:
			return 0.f;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	float PlayerInput::GetInputFire()
	{
		switch ( m_inputType )
		{
		case fan::PlayerInput::KEYBOARD_MOUSE:
			return Input::Get().Manager().GetAxis( "game_fire", m_joystickID );
		case fan::PlayerInput::JOYSTICK:
			return Input::Get().Manager().GetAxis( "gamejs_axis_fire", m_joystickID );
		default:
			return 0.f;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PlayerInput::GetInputStop()
	{
		switch ( m_inputType )
		{
		case fan::PlayerInput::KEYBOARD_MOUSE:
			return Input::Get().Manager().GetAxis( "game_axis_stop", m_joystickID ) > 0.f;
		case fan::PlayerInput::JOYSTICK:
			return Input::Get().Manager().GetAxis( "gamejs_axis_stop", m_joystickID ) > 0.f;
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
		const size_t index = Input::Get().FrameCount() % m_directionBuffer.size();
		const float x = Input::Get().Manager().GetAxis( "gamejs_x_axis_direction", m_joystickID );
		const float y = Input::Get().Manager().GetAxis( "gamejs_y_axis_direction", m_joystickID );
		m_directionBuffer[ index ] = glm::vec2( x, y );

		glm::vec2 average( 0.f, 0.f );
		for ( int dirIndex = 0; dirIndex < m_directionBuffer.size(); dirIndex++ )
		{
			average += m_directionBuffer[ dirIndex ];
		}
		average /= float( m_directionBuffer.size() );

		return average;
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::OnGui()
	{


		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			// Input type
			int type = m_inputType;
			if ( ImGui::Combo( "input type", &type, "keyboard+mouse\0joystick\0" ) )
			{
				SetInputType( InputType( type ) );
			}
			ImGui::Checkbox( "replicated", &m_isReplicated );

			ImGui::DragFloat( "fire", &m_inputData.fire );

			if ( m_inputType == JOYSTICK )
			{
				ImGui::SliderInt( "joystick ID", &m_joystickID, 0, Joystick::NUM_JOYSTICK - 1 );
			}

			// Direction buffer size
			int sizeBuffer = ( int ) m_directionBuffer.size();
			if ( ImGui::SliderInt( "direction buffer size", &sizeBuffer, 1, 64 ) )
			{
				m_directionBuffer.resize( sizeBuffer, glm::vec2( 0 ) );
			}

			// direction cut treshold
			ImGui::DragFloat( "direction cut treshold", &m_directionCutTreshold, 0.01f, 0.f, 1.f );

		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PlayerInput::Load( const Json& _json )
	{


		int tmp;
		if ( Serializable::LoadInt( _json, "direction_buffer_size", tmp ) )
		{
			m_directionBuffer.resize( tmp, glm::vec3( 0.f ) );
		}
		Serializable::LoadFloat( _json, "direction_cut_treshold", m_directionCutTreshold );
		Serializable::LoadBool( _json, "replicated", m_isReplicated );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PlayerInput::Save( Json& _json ) const
	{

		Serializable::SaveInt( _json, "direction_buffer_size", ( int ) m_directionBuffer.size() );
		Serializable::SaveFloat( _json, "direction_cut_treshold", m_directionCutTreshold );
		Serializable::SaveBool( _json, "replicated", m_isReplicated );
		return true;
	}


}