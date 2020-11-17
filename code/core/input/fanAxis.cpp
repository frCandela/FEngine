#include "core/input/fanAxis.hpp"

#include "core/fanDebug.hpp"
#include "core/memory/fanSerializable.hpp"
#include "editor/fanModals.hpp"

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