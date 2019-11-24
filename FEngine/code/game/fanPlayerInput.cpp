#include "fanGlobalIncludes.h"
#include "game/fanPlayerInput.h"

#include "core/input/fanInputManager.h"
#include "core/input/fanInput.h"
#include "core/input/fanMouse.h"
#include "core/input/fanJoystick.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanCamera.h"
#include "scene/components/fanTransform.h"

namespace fan
{
	REGISTER_TYPE_INFO( PlayerInput, TypeInfo::Flags::EDITOR_COMPONENT )

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::OnAttach()
	{
		Component::OnAttach();

		m_directionBuffer.resize(8, glm::vec2(0));
		m_lastDirection = btVector3(0,0,1.f);
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::OnDetach()
	{
		Component::OnDetach();
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
		case fan::PlayerInput::JOYSTICK:{
			float rawInput = Input::Get().Manager().GetAxis( "gamejs_axis_left", m_joystickID );
			return std::abs(rawInput) > 0.15f ? rawInput : 0.f;
		}
		default:
			return 0.f;
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
		case fan::PlayerInput::JOYSTICK: {
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
		switch ( m_inputType )
		{

		case fan::PlayerInput::KEYBOARD_MOUSE:
		{
			// Get mouse world pos
			Camera * camera = m_gameobject->GetScene()->GetMainCamera();
			btVector3 mouseWorldPos = camera->ScreenPosToRay( Mouse::GetScreenSpacePosition() ).origin;
			mouseWorldPos.setY( 0 );

			// Get mouse direction
			Transform * transform = m_gameobject->GetComponent<Transform>();
			btVector3 mouseDir = mouseWorldPos - transform->GetPosition();
			mouseDir.normalize();
			return mouseDir;
		}
		case fan::PlayerInput::JOYSTICK:
		{
			 glm::vec2 average =  GetDirectionAverage();

			btVector3 dir = btVector3( average.x, 0.f, average.y );

			if( dir.length() > m_directionCutTreshold ) { m_lastDirection = dir; }

			return m_lastDirection;
		}
		default:
			return btVector3::Zero();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::vec2 PlayerInput::GetDirectionAverage() 
	{
		const uint64_t index = Input::Get().FrameCount() % m_directionBuffer.size();
		const float x = Input::Get().Manager().GetAxis( "gamejs_x_axis_direction", m_joystickID );
		const float y = Input::Get().Manager().GetAxis( "gamejs_y_axis_direction", m_joystickID );
		m_directionBuffer[index] = glm::vec2(x,y);

		glm::vec2 average(0.f,0.f);
		for (int dirIndex = 0; dirIndex < m_directionBuffer.size() ; dirIndex++)
		{
			average += m_directionBuffer[dirIndex];
		}
		 average /= float(m_directionBuffer.size() );

		return average;
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::OnGui()
	{
		Component::OnGui();

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			// Imput type
			int type = m_inputType;
			if ( ImGui::Combo( "input type", &type, "keyboard+mouse\0joystick\0" ) )
			{
				SetInputType( InputType( type ) );
			}

			if ( m_inputType == JOYSTICK )
			{	
				ImGui::SliderInt( "joystick ID", &m_joystickID, 0, Joystick::NUM_JOYSTICK - 1 );
			}

			// Direction buffer size
			int sizeBuffer = (int) m_directionBuffer.size();
			if ( ImGui::SliderInt( "direction buffer size", &sizeBuffer, 1, 64 ) )
			{
				m_directionBuffer.resize(sizeBuffer, glm::vec2(0));
			}

			// direction cut treshold
			ImGui::DragFloat("direction cut treshold", &m_directionCutTreshold, 0.01f, 0.f, 1.f );

		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PlayerInput::Load( const Json & _json )
	{
		Component::Load( _json );

		int tmp;
		if ( LoadInt( _json, "direction_buffer_size", tmp ))
		{
			m_directionBuffer.resize(tmp, glm::vec3(0.f));
		}
		LoadFloat(_json, "direction_cut_treshold", m_directionCutTreshold );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PlayerInput::Save( Json & _json ) const
	{
		Component::Save( _json );
		SaveInt( _json, "direction_buffer_size", (int)m_directionBuffer.size() );
		SaveFloat( _json, "direction_cut_treshold", m_directionCutTreshold );
		return true;
	}


}