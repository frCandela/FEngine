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
			const float x = Input::Get().Manager().GetAxis( "gamejs_x_axis_direction" );
			const float y = Input::Get().Manager().GetAxis( "gamejs_y_axis_direction" );
			btVector3 dir = btVector3( x, 0.f, y );
			return dir.length2() > 0.01f ? dir : btVector3::Zero();
		}
		default:
			return btVector3::Zero();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	float PlayerInput::GetInputLeft()
	{		
		switch ( m_inputType )
		{
		case fan::PlayerInput::KEYBOARD_MOUSE:
			return Input::Get().Manager().GetAxis( "game_left" );
		case fan::PlayerInput::JOYSTICK:{
			float rawInput = Input::Get().Manager().GetAxis( "gamejs_axis_left" );
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
			const float x = Input::Get().Manager().GetAxis( "gamejs_x_axis_direction" );
			const float y = Input::Get().Manager().GetAxis( "gamejs_y_axis_direction" );
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
			return Input::Get().Manager().GetAxis( "game_boost" );
		case fan::PlayerInput::JOYSTICK:
			return Input::Get().Manager().GetAxis( "gamejs_axis_boost" );
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
			return Input::Get().Manager().GetAxis( "game_fire" );
		case fan::PlayerInput::JOYSTICK:
			return Input::Get().Manager().GetAxis( "gamejs_axis_fire" );
		default:
			return 0.f;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerInput::OnGui()
	{
		Component::OnGui();

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			int type = m_inputType;
			if ( ImGui::Combo( "input type", &type, "keyboard+mouse\0joystick\0" ) )
			{
				SetInputType( InputType( type ) );
			}

		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PlayerInput::Load( Json & _json )
	{
		Component::Load( _json );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PlayerInput::Save( Json & _json ) const
	{
		Component::Save( _json );
		return true;
	}


}