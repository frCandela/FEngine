#include "core/input/fanAxis.hpp"

#include "imgui/imgui.h"
#include "core/fanDebug.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	Axis::Axis()
		: mType( Type::Keyboard )
		, mName( "" )
		, mKeyPositive( Keyboard::NONE )
		, mKeyNegative( Keyboard::NONE )
		, mJoystickId( -1 )
		, mJoystickAxis( -1 )
		, mButtonNegative( -1 )
		, mButtonPositive( -1 )
	{

	}

	//========================================================================================================
	//========================================================================================================
	Axis::Axis( const std::string& _name, const Type _type, const bool _invert )
		: mType( _type )
		, mName( _name )
		, mInvert( _invert )
		, mKeyPositive( Keyboard::NONE )
		, mKeyNegative( Keyboard::NONE )
		, mJoystickId( -1 )
		, mJoystickAxis( Joystick::AXIS_NONE )
	{

	}

	//========================================================================================================
	//========================================================================================================
	bool Axis::Load( const Json& _json )
	{
		int type = -1;
		if ( Serializable::LoadInt( _json, "type", type ) &&
		   ( type == Type::JoystickAxis || type == Type::Keyboard || type == Type::JoystickButton ) )
		{
            mType = Type( type );

			Serializable::LoadString( _json, "name", mName );
			Serializable::LoadBool( _json, "invert", mInvert );

			switch ( mType )
			{
			case fan::Axis::Keyboard:
				Serializable::LoadInt( _json, "key_negative", mKeyNegative );
				Serializable::LoadInt( _json, "key_positive", mKeyPositive );
				break;
			case fan::Axis::JoystickAxis:
				Serializable::LoadInt( _json, "joystick_id", mJoystickId );
				Serializable::LoadInt( _json, "joystick_axis", mJoystickAxis );
				break;
			case fan::Axis::JoystickButton:
				Serializable::LoadInt( _json, "joystick_id", mJoystickId );
				Serializable::LoadInt( _json, "joystick_negative", mButtonNegative );
				Serializable::LoadInt( _json, "joystick_positive", mButtonPositive );
				break;
			default:
				Debug::Warning( "Axis load: invalid axis type" );
				return false;
			}

			return true;
		}
		return false;
	}

	//========================================================================================================
	//========================================================================================================
	bool Axis::Save( Json& _json ) const
	{
		Serializable::SaveInt( _json, "type", mType );
		Serializable::SaveString( _json, "name", mName );
		Serializable::SaveBool( _json, "invert", mInvert );

		switch ( mType )
		{
		case fan::Axis::Keyboard:
			Serializable::SaveInt( _json, "key_negative", mKeyNegative );
			Serializable::SaveInt( _json, "key_positive", mKeyPositive );
			break;
		case fan::Axis::JoystickAxis:
			Serializable::SaveInt( _json, "joystick_id", mJoystickId );
			Serializable::SaveInt( _json, "joystick_axis", mJoystickAxis );
			break;
		case fan::Axis::JoystickButton:
			Serializable::SaveInt( _json, "joystick_id", mJoystickId );
			Serializable::SaveInt( _json, "joystick_negative", mButtonNegative );
			Serializable::SaveInt( _json, "joystick_positive", mButtonPositive );
			break;
		default:
			Debug::Warning( "Axis save: invalid axis type" );
			return false;
		}

		return true;
	}

	//========================================================================================================
	//========================================================================================================
	void Axis::SetFromKeyboardKeys( const Keyboard::Key _keyPositive, const Keyboard::Key _keyNegative )
	{
        mType        = Keyboard;
        mKeyPositive = _keyPositive;
        mKeyNegative = _keyNegative;
	}

	//========================================================================================================
    //========================================================================================================
    void Axis::SetFromJoystickAxis( const Joystick::JoystickID _joystickID,
                                    const Joystick::Axis _joystickAxis )
	{
        mType         = JoystickAxis;
        mJoystickId   = _joystickID;
        mJoystickAxis = _joystickAxis;
	}

	//========================================================================================================
	//========================================================================================================
    void Axis::SetFromJoystickButtons( const Joystick::JoystickID _joystickID,
                                       const Joystick::Button _buttonPositive, const
                                       Joystick::Button _buttonNegative )
	{
        mType           = JoystickButton;
        mJoystickId     = _joystickID;
        mButtonPositive = _buttonPositive;
        mButtonNegative = _buttonNegative;
	}

	//========================================================================================================
	//========================================================================================================
	float Axis::GetValue( const int _joystickIDOverride ) const
	{
		const float invertValue = mInvert ? -1.f : 1.f;
		const int joystickID = _joystickIDOverride >= 0 ? _joystickIDOverride : mJoystickId;

		switch ( mType )
        {
            case fan::Axis::Keyboard:
                return invertValue *
                       ( ( Keyboard::IsKeyDown( mKeyPositive ) ? 1.f : 0.f ) +
                         ( Keyboard::IsKeyDown( mKeyNegative ) ? -1.f : 0.f ) );
            case fan::Axis::JoystickAxis:
                return -invertValue * Joystick::Get().GetAxis( joystickID, mJoystickAxis );
            case fan::Axis::JoystickButton:
                return invertValue *
                       ( ( Joystick::Get().GetButton( joystickID, mButtonPositive ) ? 1.f : 0.f ) +
                         ( Joystick::Get().GetButton( joystickID, mButtonNegative ) ? -1.f : 0.f ) );
            default:
                fanAssert( false );
                return 0.f;
        }
	}

	//========================================================================================================
	//========================================================================================================
	std::string	Axis::GetKeyPositiveName() const
	{
		if ( mKeyPositive != Keyboard::NONE ) { return Keyboard::GetKeyName( mKeyPositive ); }
		else { return Joystick::Get().GetButtonName( mButtonPositive ); }
	}

	//========================================================================================================
	//========================================================================================================
	std::string Axis::GetKeyNegativeName() const
	{
		if ( mKeyNegative != Keyboard::NONE ) { return Keyboard::GetKeyName( mKeyPositive ); }
		else { return Joystick::Get().GetButtonName( mButtonNegative ); }
	}
}

namespace ImGui
{
	//========================================================================================================
	//========================================================================================================
	bool FanAxis( const char* _label, fan::Axis* _axis )
	{
		using namespace fan;

		ImGui::PushID( _axis );
		{
			ImGui::PushItemWidth( 150 );

			int type = _axis->GetType();
            if( ImGui::Combo( "", &type, "keyboard\0joystick axis\0joystick buttons\0" ) &&
                Axis::Type( type ) != _axis->GetType() )
			{
				*_axis = Axis( _axis->GetName(), Axis::Type( type ) );
			}
			ImGui::PopItemWidth();

			ImGui::SameLine();

			bool invert = _axis->GetInvert();
			if ( ImGui::Checkbox( "##invert", &invert ) )
			{
				_axis->SetInvert( invert );
			}
			ImGui::SameLine();

			static Axis* sCapturingAxis = nullptr;
			static bool sPositiveKeyCaptured = true;


			bool openPopup[ 3 ] = { false,false,false };

			switch ( _axis->GetType() )
			{
			case fan::Axis::Keyboard:
			{
				// keyboard button +
				if ( ImGui::Button( Keyboard::GetKeyName( _axis->GetKeyPositive() ).c_str() ) )
				{
					openPopup[ Axis::Keyboard ] = true;
                    sCapturingAxis       = _axis;
                    sPositiveKeyCaptured = true;
				}
				// reset +
                if( ImGui::IsItemClicked( 1 ) )
                {
                    _axis->SetFromKeyboardKeys( Keyboard::NONE, _axis->GetKeyNegative() );
                }

                // keyboard button -
                ImGui::SameLine();
                const std::string name_neg = Keyboard::GetKeyName( _axis->GetKeyNegative() ) +
                                             "##key_but_neg";
                if( ImGui::Button( name_neg.c_str() ) )
                {
                    openPopup[Axis::Keyboard] = true;
                    sCapturingAxis       = _axis;
                    sPositiveKeyCaptured = false;
                }
                // reset -
                if( ImGui::IsItemClicked( 1 ) )
                {
                    _axis->SetFromKeyboardKeys( _axis->GetKeyPositive(), Keyboard::NONE );
                }
            }
                    break;

			case fan::Axis::JoystickAxis:
				// Joystick buttons
				if ( ImGui::Button( Joystick::Get().GetAxisName( _axis->GetJoystickAxis() ).c_str() ) )
				{
					openPopup[ Axis::JoystickAxis ] = true;
                    sCapturingAxis = _axis;
				}
				// reset
				if ( ImGui::IsItemClicked( 1 ) ) { _axis->SetFromJoystickAxis( -1, Joystick::AXIS_NONE ); }
				break;

			case fan::Axis::JoystickButton:
			{
				// Joystick button +
				ImGui::PushID( "joystick_button_pos" );
				if ( ImGui::Button( Joystick::Get().GetButtonName( _axis->GetButtonPositive() ).c_str() ) )
				{
					openPopup[ Axis::JoystickButton ] = true;
                    sCapturingAxis       = _axis;
                    sPositiveKeyCaptured = true;
				} ImGui::PopID();
				// reset +
				if ( ImGui::IsItemClicked( 1 ) )
				{
				    _axis->SetFromJoystickButtons( _axis->GetJoystickID(), -1, _axis->GetButtonNegative() );
				}

				// Joystick button -
				ImGui::SameLine();
				const std::string name_neg = Joystick::Get().GetButtonName( _axis->GetButtonNegative() ) +
				                                                            "##joy_but_neg";
				if ( ImGui::Button( name_neg.c_str() ) )
				{
					openPopup[ Axis::JoystickButton ] = true;
                    sCapturingAxis       = _axis;
                    sPositiveKeyCaptured = false;
				}
				// reset
				if ( ImGui::IsItemClicked( 1 ) )
				{
				    _axis->SetFromJoystickButtons( _axis->GetJoystickID(), _axis->GetButtonPositive(), -1 );
				}

			} break;
			default:
                fanAssert( false );
				break;
			}

			ImGui::SameLine();
			ImGui::Text( _label );

			// Capture joystick axis popup
			if ( openPopup[ Axis::JoystickAxis ] ) { ImGui::OpenPopup( "capture_joystick_axis" ); }
			if ( ImGui::BeginPopup( "capture_joystick_axis" ) )
			{
				if ( sCapturingAxis == _axis )
				{
					ImGui::Text( "PRESS ANY KEY" );

					// Look for a pressed axis
					for ( int joystickIndex = 0; joystickIndex < Joystick::NUM_JOYSTICK; joystickIndex++ )
					{
						if ( Joystick::Get().IsConnected( joystickIndex ) &&
						     Joystick::Get().IsGamepad( joystickIndex ) )
						{
							const std::vector< Joystick::Axis >& axes = Joystick::Get().GetGamepadAxisList();
							for ( int axisIndex = 0; axisIndex < (int) axes.size(); axisIndex++ )
							{
								float axisValue = Joystick::Get().GetAxis( joystickIndex, axes[ axisIndex ] );
								if ( axisValue == 1.f )
								{
									*sCapturingAxis = Axis( sCapturingAxis->GetName(),
									                        fan::Axis::JoystickAxis );
									sCapturingAxis->SetFromJoystickAxis( joystickIndex, axisIndex );
                                    sCapturingAxis = nullptr;
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
			if ( openPopup[ Axis::Keyboard ] ) { ImGui::OpenPopup( "capture_keyboard_axis" ); }
			if ( ImGui::BeginPopup( "capture_keyboard_axis" ) )
			{
				if ( sCapturingAxis == _axis )
				{
					ImGui::Text( "PRESS ANY KEY" );
					Keyboard& toto = Keyboard::Get();
					const std::vector<Keyboard::Key>& keysList = toto.GetKeysList();
					for ( int keyIndex = 0; keyIndex < (int)keysList.size(); keyIndex++ )
					{
						if ( Keyboard::IsKeyDown( keysList[ keyIndex ] ) )
						{
							Keyboard::Key positiveKey = sPositiveKeyCaptured ?
							                            keysList[ keyIndex ] :
							                            sCapturingAxis->GetKeyPositive();
							Keyboard::Key negativeKey = !sPositiveKeyCaptured ?
							                            keysList[ keyIndex ] :
							                            sCapturingAxis->GetKeyNegative();
							*sCapturingAxis = Axis( sCapturingAxis->GetName(), fan::Axis::Keyboard );
							sCapturingAxis->SetFromKeyboardKeys( positiveKey, negativeKey );
                            sCapturingAxis = nullptr;
							ImGui::CloseCurrentPopup();
							break;
						}
					}
				}
				ImGui::EndPopup();
			}

			// Capture joystick button popup
			if ( openPopup[ Axis::JoystickButton ] ) { ImGui::OpenPopup( "capture_joystick_button" ); }
			if ( ImGui::BeginPopup( "capture_joystick_button" ) )
			{
				if ( sCapturingAxis == _axis )
				{
					ImGui::Text( "PRESS ANY KEY" );

					// Look for a pressed axis
					for ( int joystickIndex = 0; joystickIndex < Joystick::NUM_JOYSTICK; joystickIndex++ )
					{
						if ( Joystick::Get().IsConnected( joystickIndex ) &&
						     Joystick::Get().IsGamepad( joystickIndex ) )
						{
							const std::vector< Joystick::Button >& buttons = Joystick::Get().GetGamepadButtonsList();
							for ( int buttonIndex = 0; buttonIndex < (int)buttons.size(); buttonIndex++ )
							{
                                if( Joystick::Get().GetButton( joystickIndex, buttons[buttonIndex] ) )
                                {
                                    Joystick::Button positiveButton = sPositiveKeyCaptured ?
                                            buttons[buttonIndex] :
                                            sCapturingAxis->GetButtonPositive();
                                    Joystick::Button negativeButton = !sPositiveKeyCaptured ?
                                            buttons[buttonIndex] :
                                            sCapturingAxis->GetButtonNegative();
                                    sCapturingAxis->SetFromJoystickButtons( joystickIndex,
                                                                            positiveButton,
                                                                            negativeButton );
                                    sCapturingAxis = nullptr;
                                    ImGui::CloseCurrentPopup();
                                    break;
								}
							}
						}
					}
				}
				ImGui::EndPopup();
			}
		}
		ImGui::PopID();
		return false;
	}
}