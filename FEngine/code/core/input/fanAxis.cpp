#include "fanGlobalIncludes.h"
#include "core/input/fanAxis.h"

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
		, m_joystickAxis( Joystick::AXIS_NONE )
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
	bool Axis::Load( Json & _json )
	{
		int type = -1;
		if ( LoadInt( _json, "type", type ) && ( type == Type::JOYSTICK || type == Type::KEYBOARD ) )
		{
			m_type = Type( type );

			LoadString( _json, "name", m_name );
			LoadBool(_json, "invert", m_invert );

			if ( m_type == JOYSTICK )
			{
				LoadInt( _json, "joystick_id", m_joystickID );
				LoadInt( _json, "joystick_axis", m_joystickAxis );
			}
			else if ( m_type == KEYBOARD )
			{
				LoadInt( _json, "key_negative", m_keyNegative );
				LoadInt( _json, "key_positive", m_keyPositive );
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

		if ( m_type == JOYSTICK )
		{
			SaveInt( _json, "joystick_id", m_joystickID );
			SaveInt( _json, "joystick_axis", m_joystickAxis );
		}
		else if ( m_type == KEYBOARD )
		{
			SaveInt( _json, "key_negative", m_keyNegative );
			SaveInt( _json, "key_positive", m_keyPositive );
		}
		else
		{
			return false;
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Axis::SetKeyboardKeys( const Keyboard::Key _keyPositive, const Keyboard::Key _keyNegative )
	{
		m_keyPositive = _keyPositive;
		m_keyNegative = _keyNegative;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Axis::SetJoystickKeys( const Joystick::JoystickID _joystickID, const Joystick::Axis _joystickAxis )
	{
		m_joystickID = _joystickID;
		m_joystickAxis = _joystickAxis;
	}

	//================================================================================================================================
	//================================================================================================================================
	float Axis::GetValue() const
	{
		float invertValue = m_invert ? -1.f : 1.f;
		if ( m_type == JOYSTICK )
		{
			return invertValue * Joystick::Get().GetAxis( m_joystickID, m_joystickAxis );
		}
		else if ( m_type == KEYBOARD )
		{
			return invertValue * ( ( Keyboard::IsKeyDown( m_keyPositive ) ? 1.f : 0.f ) + ( Keyboard::IsKeyDown( m_keyNegative ) ? -1.f : 0.f ) );
		}
		else
		{
			assert( false );
			return 0.f;
		}
	}
}

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
			if ( ImGui::Combo( "", &type, "buttons\0joystick\0" ) &&  Axis::Type(type) != _axis->GetType() )
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

		
			if ( _axis->GetType() == Axis::JOYSTICK )
			{
				// Joystick buttons
				if ( ImGui::Button( Joystick::Get().GetAxisName( _axis->GetJoystickAxis() ).c_str() ) )
				{
					ImGui::OpenPopup( "capture_joystick_axis" );
					s_capturingAxis = _axis;
				}			
				// reset
				if ( ImGui::IsItemClicked( 1 ) ){ _axis->SetJoystickKeys(  -1, Joystick::AXIS_NONE );	} 
			}		
			else if ( _axis->GetType() == Axis::KEYBOARD ) 
			{
				// keyboard button +
				if ( ImGui::Button( Keyboard::GetKeyName( _axis->GetKeyPositive() ).c_str() ) )
				{
					ImGui::OpenPopup( "capture_keyboard_axis" );
					s_capturingAxis = _axis;
					s_positiveKeyCaptured = true;
				}
				// reset +
				if ( ImGui::IsItemClicked( 1 ) ) { _axis->SetKeyboardKeys( Keyboard::NONE, _axis->GetKeyNegative() ); }

				// keyboard button -
				ImGui::SameLine();
				if ( ImGui::Button( Keyboard::GetKeyName( _axis->GetKeyNegative() ).c_str() ) )
				{
					{
						ImGui::OpenPopup( "capture_keyboard_axis" );
						s_capturingAxis = _axis;
						s_positiveKeyCaptured = false;
					}
				}
				// reset -
				if ( ImGui::IsItemClicked( 1 ) ) { _axis->SetKeyboardKeys( _axis->GetKeyPositive(), Keyboard::NONE ); }

			}
			else
			{
				assert( false );
			}

			ImGui::SameLine();
			ImGui::Text(_label);

			// Capture joystick axis popup
			if ( ImGui::BeginPopup( "capture_joystick_axis" ) )
			{
				if ( s_capturingAxis == _axis )
				{
					ImGui::Text( "PRESS ANY KEY" );

					// Look for a pressed axis
					for ( int joystickIndex = 0; joystickIndex <= GLFW_JOYSTICK_LAST; joystickIndex++ )
					{
						if ( Joystick::Get().IsConnected( joystickIndex ) && Joystick::Get().IsGamepad( joystickIndex ) )
						{
							const std::vector< Joystick::Axis >& axes = Joystick::Get().GetGamepadAxisList();
							for ( int axisIndex = 0; axisIndex < axes.size(); axisIndex++ )
							{
								float axisValue = Joystick::Get().GetAxis( joystickIndex, axes[axisIndex] );
								if ( axisValue == 1.f )
								{
									*s_capturingAxis = Axis( s_capturingAxis->GetName(), fan::Axis::JOYSTICK );
									s_capturingAxis->SetJoystickKeys(joystickIndex, axisIndex);
									s_capturingAxis = nullptr;
									ImGui::CloseCurrentPopup();
								}
							}
						}
					}
				}
				ImGui::EndPopup();
			}

			// Capture keyboard axis popup
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
							s_capturingAxis->SetKeyboardKeys( positiveKey, negativeKey );
							s_capturingAxis = nullptr;
							ImGui::CloseCurrentPopup();
							break;
						}
					}
				}
				ImGui::EndPopup();
			}

		} ImGui::PopID();

		return false;

	}
}