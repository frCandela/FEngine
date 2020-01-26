#include "core/input/fanAxis.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Axis::Axis()
		:m_type( Type::KEYBOARD )
		, m_name( "" )
		, m_keyPositive( Keyboard::NONE )
		, m_keyNegative( Keyboard::NONE )
		, m_joystickID( -1 )
		, m_joystickAxis( -1 )
		, m_buttonNegative( -1 )
		, m_buttonPositive( -1 )
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	Axis::Axis(const std::string& _name, const Type _type, const bool _invert )
		: m_type( _type )
		, m_name( _name )
		, m_invert( _invert )
		, m_keyPositive( Keyboard::NONE )
		, m_keyNegative( Keyboard::NONE )
		, m_joystickID( -1 )
		, m_joystickAxis( Joystick::AXIS_NONE )
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	bool Axis::Load( const Json & _json )
	{
		int type = -1;
		if ( LoadInt( _json, "type", type ) && ( type == Type::JOYSTICK_AXIS || type == Type::KEYBOARD || type == Type::JOYSTICK_BUTTONS ) )
		{
			m_type = Type( type );

			LoadString( _json, "name", m_name );
			LoadBool(_json, "invert", m_invert );


			switch ( m_type )
			{
			case fan::Axis::KEYBOARD:
				LoadInt( _json, "key_negative", m_keyNegative );
				LoadInt( _json, "key_positive", m_keyPositive );
				break;
			case fan::Axis::JOYSTICK_AXIS:
				LoadInt( _json, "joystick_id", m_joystickID );
				LoadInt( _json, "joystick_axis", m_joystickAxis );
				break;
			case fan::Axis::JOYSTICK_BUTTONS:
				LoadInt( _json, "joystick_id", m_joystickID );
				LoadInt( _json, "joystick_negative", m_buttonNegative );
				LoadInt( _json, "joystick_positive", m_buttonPositive );
				break;
			default:
				Debug::Warning("Axis load: invalid axis type");
				return false;
			}

			return true;
		}
		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Axis::Save( Json & _json ) const
	{
		SaveInt( _json, "type", m_type );
		SaveString( _json, "name", m_name );
		SaveBool(_json, "invert", m_invert );


		switch ( m_type )
		{
		case fan::Axis::KEYBOARD:
			SaveInt( _json, "key_negative", m_keyNegative );
			SaveInt( _json, "key_positive", m_keyPositive );
			break;
		case fan::Axis::JOYSTICK_AXIS:
			SaveInt( _json, "joystick_id", m_joystickID );
			SaveInt( _json, "joystick_axis", m_joystickAxis );
			break;
		case fan::Axis::JOYSTICK_BUTTONS:
			SaveInt( _json, "joystick_id", m_joystickID );
			SaveInt( _json, "joystick_negative", m_buttonNegative );
			SaveInt( _json, "joystick_positive", m_buttonPositive );
			break;
		default:
			Debug::Warning("Axis save: invalid axis type");
			return false;
		}

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Axis::SetFromKeyboardKeys( const Keyboard::Key _keyPositive, const Keyboard::Key _keyNegative )
	{
		m_type = KEYBOARD;
		m_keyPositive = _keyPositive;
		m_keyNegative = _keyNegative;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Axis::SetFromJoystickAxis( const Joystick::JoystickID _joystickID, const Joystick::Axis _joystickAxis )
	{
		m_type = JOYSTICK_AXIS;
		m_joystickID = _joystickID;
		m_joystickAxis = _joystickAxis;
	}

	//================================================================================================================================
	//================================================================================================================================
	void   Axis::SetFromJoystickButtons( const Joystick::JoystickID _joystickID, const Joystick::Button _buttonPositive, const Joystick::Button _buttonNegative )
	{
		m_type = JOYSTICK_BUTTONS;
		m_joystickID = _joystickID;
		m_buttonPositive = _buttonPositive;
		m_buttonNegative = _buttonNegative;
	}

	//================================================================================================================================
	//================================================================================================================================
	float Axis::GetValue( const int _joystickIDOverride ) const
	{
		const float invertValue = m_invert ? -1.f : 1.f;
		const int joystickID = _joystickIDOverride >= 0 ? _joystickIDOverride : m_joystickID;

		switch ( m_type )
		{
		case fan::Axis::KEYBOARD:
			return invertValue * ( ( Keyboard::IsKeyDown( m_keyPositive ) ? 1.f : 0.f ) + ( Keyboard::IsKeyDown( m_keyNegative ) ? -1.f : 0.f ) );
		case fan::Axis::JOYSTICK_AXIS:
			return -invertValue * Joystick::Get().GetAxis( joystickID, m_joystickAxis );
		case fan::Axis::JOYSTICK_BUTTONS:
			return invertValue * ( ( Joystick::Get().GetButton( joystickID, m_buttonPositive ) ? 1.f : 0.f ) + ( Joystick::Get().GetButton( joystickID, m_buttonNegative ) ? -1.f : 0.f ) );
		default:
			assert( false );
			return 0.f;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	std::string	Axis::GetKeyPositiveName() const
	{
		if( m_keyPositive != Keyboard::NONE ){ return Keyboard::GetKeyName(m_keyPositive); }
		else{ return Joystick::Get().GetButtonName(m_buttonPositive); }
	}

	//================================================================================================================================
	//================================================================================================================================
	std::string Axis::GetKeyNegativeName() const
	{
		if ( m_keyNegative != Keyboard::NONE ) { return Keyboard::GetKeyName( m_keyPositive ); }
		else { return Joystick::Get().GetButtonName( m_buttonNegative ); }
	}
}
/*
namespace ImGui
{
	//================================================================================================================================
	//================================================================================================================================
	bool FanAxis( const char * _label, fan::Axis* _axis )
	{
		using namespace fan;

		ImGui::PushID( _axis );
		{
			ImGui::PushItemWidth( 150 );

			int type = _axis->GetType();
			if ( ImGui::Combo( "", &type, "keyboard\0joystick axis\0joystick buttons\0" ) &&  Axis::Type(type) != _axis->GetType() )
			{
				*_axis = Axis(_axis->GetName(),  Axis::Type(type) );
			}
			ImGui::PopItemWidth();
		
			ImGui::SameLine();

			bool invert = _axis->GetInvert();
			if ( ImGui::Checkbox( "##invert", &invert ) )
			{
				_axis->SetInvert(invert);
			}
			ImGui::SameLine();

			static Axis* s_capturingAxis = nullptr;
			static bool s_positiveKeyCaptured = true;


			bool openPopup[3] = {false,false,false};

			switch ( _axis->GetType() )
			{
				case fan::Axis::KEYBOARD: {
					// keyboard button +
					if ( ImGui::Button( Keyboard::GetKeyName( _axis->GetKeyPositive() ).c_str() ) )
					{
						openPopup[Axis::KEYBOARD] = true;
						s_capturingAxis = _axis;
						s_positiveKeyCaptured = true;
					}
					// reset +
					if ( ImGui::IsItemClicked( 1 ) ) { _axis->SetFromKeyboardKeys( Keyboard::NONE, _axis->GetKeyNegative() ); }

					// keyboard button -
					ImGui::SameLine();
					const std::string name_neg = Keyboard::GetKeyName( _axis->GetKeyNegative() ) + "##key_but_neg";
					if ( ImGui::Button( name_neg.c_str() ) )
					{
						{
							openPopup[Axis::KEYBOARD] = true;
							s_capturingAxis = _axis;
							s_positiveKeyCaptured = false;
						}
					}
					// reset -
					if ( ImGui::IsItemClicked( 1 ) ) { _axis->SetFromKeyboardKeys( _axis->GetKeyPositive(), Keyboard::NONE ); }
				}break;

				case fan::Axis::JOYSTICK_AXIS:
					// Joystick buttons
					if ( ImGui::Button( Joystick::Get().GetAxisName( _axis->GetJoystickAxis() ).c_str() ) )
					{
						openPopup[Axis::JOYSTICK_AXIS] = true;
						s_capturingAxis = _axis;
					}
					// reset
					if ( ImGui::IsItemClicked( 1 ) ) { _axis->SetFromJoystickAxis( -1, Joystick::AXIS_NONE ); }
					break;

				case fan::Axis::JOYSTICK_BUTTONS: {
					// Joystick button +
					ImGui::PushID("joystick_button_pos");
					if ( ImGui::Button( Joystick::Get().GetButtonName( _axis->GetButtonPositive() ).c_str() ) )
					{
						openPopup[Axis::JOYSTICK_BUTTONS] = true;
						s_capturingAxis = _axis;
						s_positiveKeyCaptured = true;
					} ImGui::PopID();
					// reset +
					if ( ImGui::IsItemClicked( 1 ) ) { _axis->SetFromJoystickButtons( _axis->GetJoystickID(),-1, _axis->GetButtonNegative() ); }

					// Joystick button -
					ImGui::SameLine();
					const std::string name_neg = Joystick::Get().GetButtonName( _axis->GetButtonNegative() ) + "##joy_but_neg";
					if ( ImGui::Button( name_neg.c_str() ) )
					{						
						openPopup[Axis::JOYSTICK_BUTTONS] = true;
						s_capturingAxis = _axis;
						s_positiveKeyCaptured = false;	
					}
					// reset
					if ( ImGui::IsItemClicked( 1 ) ) { _axis->SetFromJoystickButtons( _axis->GetJoystickID(), _axis->GetButtonPositive(), -1 ); }
					
				}break;

				default:
					assert( false );
					break;
				}

			ImGui::SameLine();
			ImGui::Text(_label);

			// Capture joystick axis popup
			if ( openPopup[Axis::JOYSTICK_AXIS] ) { ImGui::OpenPopup( "capture_joystick_axis" ); }
			if ( ImGui::BeginPopup( "capture_joystick_axis" ) )
			{
				if ( s_capturingAxis == _axis )
				{
					ImGui::Text( "PRESS ANY KEY" );

					// Look for a pressed axis
					for ( int joystickIndex = 0; joystickIndex < Joystick::NUM_JOYSTICK ; joystickIndex++ )
					{
						if ( Joystick::Get().IsConnected( joystickIndex ) && Joystick::Get().IsGamepad( joystickIndex ) )
						{
							const std::vector< Joystick::Axis >& axes = Joystick::Get().GetGamepadAxisList();
							for ( int axisIndex = 0; axisIndex < axes.size(); axisIndex++ )
							{
								float axisValue = Joystick::Get().GetAxis( joystickIndex, axes[axisIndex] );
								if ( axisValue == 1.f )
								{
									*s_capturingAxis = Axis( s_capturingAxis->GetName(), fan::Axis::JOYSTICK_AXIS );
									s_capturingAxis->SetFromJoystickAxis(joystickIndex, axisIndex);
									s_capturingAxis = nullptr;
									ImGui::CloseCurrentPopup();
									break;
								}
							}
						}
					}
				}
				ImGui::EndPopup();
			}

			// Capture keyboard axis popup
			if ( openPopup[Axis::KEYBOARD] ) { ImGui::OpenPopup( "capture_keyboard_axis" ); }
			if ( ImGui::BeginPopup( "capture_keyboard_axis" ) )
			{
				if ( s_capturingAxis == _axis )
				{
					ImGui::Text( "PRESS ANY KEY" );
					Keyboard& toto = Keyboard::Get();
					const std::vector<Keyboard::Key>& keysList = toto.GetKeysList();
					for ( int keyIndex = 0; keyIndex < keysList.size(); keyIndex++ )
					{
						if ( Keyboard::IsKeyDown( keysList[keyIndex] ) )
						{
							Keyboard::Key positiveKey = s_positiveKeyCaptured ? keysList[keyIndex] : s_capturingAxis->GetKeyPositive();
							Keyboard::Key negativeKey = ! s_positiveKeyCaptured ? keysList[keyIndex] : s_capturingAxis->GetKeyNegative();
							*s_capturingAxis = Axis( s_capturingAxis->GetName(), fan::Axis::KEYBOARD );
							s_capturingAxis->SetFromKeyboardKeys( positiveKey, negativeKey );
							s_capturingAxis = nullptr;
							ImGui::CloseCurrentPopup();
							break;
						}
					}
				}
				ImGui::EndPopup();
			}

			// Capture joystick button popup
			if ( openPopup[Axis::JOYSTICK_BUTTONS] ) { ImGui::OpenPopup( "capture_joystick_button" ); }
			if ( ImGui::BeginPopup( "capture_joystick_button" ) )
			{
				if ( s_capturingAxis == _axis )
				{
					ImGui::Text( "PRESS ANY KEY" );

					// Look for a pressed axis
					for ( int joystickIndex = 0; joystickIndex < Joystick::NUM_JOYSTICK ; joystickIndex++ )
					{
						if ( Joystick::Get().IsConnected( joystickIndex ) && Joystick::Get().IsGamepad( joystickIndex ) )
						{
							const std::vector< Joystick::Button >& buttons = Joystick::Get().GetGamepadButtonsList();
							for ( int buttonIndex = 0; buttonIndex < buttons.size(); buttonIndex++ )
							{								
								if ( Joystick::Get().GetButton( joystickIndex, buttons[buttonIndex] ) )
								{
									Joystick::Button positiveButton = s_positiveKeyCaptured ? buttons[buttonIndex] : s_capturingAxis->GetButtonPositive();
									Joystick::Button negativeButton = !s_positiveKeyCaptured ? buttons[buttonIndex] : s_capturingAxis->GetButtonNegative();
									s_capturingAxis->SetFromJoystickButtons(joystickIndex, positiveButton, negativeButton );
									s_capturingAxis = nullptr;
									ImGui::CloseCurrentPopup();
									break;
								}
							}
						}
					}
				}
				ImGui::EndPopup();
			}

		} ImGui::PopID();

		return false;

	}
}*/