#pragma once

#include "core/input/fanKeyboard.h"
#include "core/input/fanJoystick.h"
#include "core/fanISerializable.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================	
	class Axis : public ISerializable
	{
	public:
		enum Type { KEYBOARD, JOYSTICK_AXIS, JOYSTICK_BUTTONS };
		Axis();
		Axis( const std::string& _name, const Type _type, const bool _invert = false );		

		void					SetInvert( const bool _invert ) { m_invert = _invert; }

		std::string				GetName() const { return m_name; };
		float					GetValue() const;
		bool					GetInvert() const { return m_invert; };
		Type					GetType() const { return m_type; }
		Keyboard::Key			GetKeyPositive() const { return m_keyPositive; }
		Keyboard::Key			GetKeyNegative() const { return m_keyNegative; }
		Joystick::Button		GetButtonPositive() const { return m_buttonPositive; }
		Joystick::Button		GetButtonNegative() const { return m_buttonNegative; }
		Joystick::JoystickID	GetJoystickID() const { return m_joystickID; }
		Joystick::Axis			GetJoystickAxis() const { return m_joystickAxis; }
		std::string				GetKeyPositiveName() const;
		std::string				GetKeyNegativeName() const;
		
		bool  Load( const Json & _json ) override;
		bool  Save( Json & _json ) const override;
		
		void  SetFromKeyboardKeys( const Keyboard::Key _keyPositive, const Keyboard::Key _keyNegative );
		void  SetFromJoystickButtons( const Joystick::JoystickID _joystickID, const Joystick::Button _buttonPositive, const Joystick::Button _buttonNegative );
		void  SetFromJoystickAxis(    const Joystick::JoystickID _joystickID, const Joystick::Axis _joystickAxis );

	private:
		bool m_invert = false;
		std::string m_name;
		Type m_type;

		// KEYBOARD
		Keyboard::Key    m_keyPositive = Keyboard::NONE;
		Keyboard::Key    m_keyNegative = Keyboard::NONE;

		// JoystickID
		Joystick::JoystickID m_joystickID = -1;

		// JOYSTICK_BUTTONS
		Joystick::Button m_buttonPositive = -1;
		Joystick::Button m_buttonNegative = -1;		

		// JOYSTICK_AXIS
		Joystick::Axis		 m_joystickAxis = -1;
	};
}

namespace ImGui
{	
	//================================================================================================================================
	//================================================================================================================================
	bool FanAxis( const char * _label, fan::Axis* _axis );
}