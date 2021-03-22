#pragma once

#include "core/input/fanKeyboard.hpp"
#include "core/input/fanJoystick.hpp"

namespace fan
{
	//========================================================================================================
	// an axis is an input value between -1.f and 1.f
	// it can be bound to an analog joystick on a game pad or to two keys on a keyboard
	//========================================================================================================
	class Axis
	{
	public:
		enum Type { Keyboard, JoystickAxis, JoystickButton };
		Axis();
		Axis( const std::string& _name, const Type _type, const bool _invert = false );

		void					SetInvert( const bool _invert ) { mInvert = _invert; }

		float					GetValue( const int _joystickIDOverride = -1 ) const;
		std::string				GetName() const				{ return mName; };
		bool					GetInvert() const			{ return mInvert; };
		Type					GetType() const				{ return mType; }
		Keyboard::Key			GetKeyPositive() const		{ return mKeyPositive; }
		Keyboard::Key			GetKeyNegative() const		{ return mKeyNegative; }
		Joystick::Button		GetButtonPositive() const	{ return mButtonPositive; }
		Joystick::Button		GetButtonNegative() const	{ return mButtonNegative; }
		Joystick::JoystickID	GetJoystickID() const		{ return mJoystickId; }
		Joystick::Axis			GetJoystickAxis() const		{ return mJoystickAxis; }
		std::string				GetKeyPositiveName() const;
		std::string				GetKeyNegativeName() const;

		bool  Load( const Json& _json );
		bool  Save( Json& _json ) const;

		void  SetFromKeyboardKeys( const Keyboard::Key _keyPositive, const Keyboard::Key _keyNegative );
        void SetFromJoystickButtons( const Joystick::JoystickID _joystickID,
                                     const Joystick::Button _buttonPositive,
                                     const Joystick::Button _buttonNegative );
        void SetFromJoystickAxis( const Joystick::JoystickID _joystickID,
                                  const Joystick::Axis _joystickAxis );

	private:
		bool        mInvert;
		std::string mName;
		Type        mType;

		Keyboard::Key           mKeyPositive = Keyboard::NONE;
		Keyboard::Key           mKeyNegative = Keyboard::NONE;
		Joystick::JoystickID    mJoystickId = -1;
        Joystick::Axis          mJoystickAxis = -1;
        Joystick::Button        mButtonPositive = -1;
        Joystick::Button        mButtonNegative = -1;
	};
}