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
		enum Type { KEYBOARD, JOYSTICK };
		Axis();
		Axis( const std::string& _name, const Type _type );		

		std::string				GetName() const { return m_name; };
		float					GetValue() const;
		Type					GetType() const { return m_type; }
		Keyboard::Key			GetKeyPositive() const { return m_keyPositive; }
		Keyboard::Key			GetKeyNegative() const { return m_keyNegative; }
		Joystick::JoystickID	GetJoystickID() const { return m_joystickID; }
		Joystick::Axis			GetJoystickAxis() const { return m_joystickAxis; }

		
		bool  Load( Json & _json ) override;
		bool  Save( Json & _json ) const override;
		
		void  SetKeyboardKeys( const Keyboard::Key _keyPositive, const Keyboard::Key _keyNegative );
		void  SetJoystickKeys( const Joystick::JoystickID _joystickID, const Joystick::Axis _joystickAxis );



	private:

		std::string m_name;
		Type m_type;

		Keyboard::Key m_keyPositive;
		Keyboard::Key m_keyNegative;

		Joystick::JoystickID m_joystickID;
		Joystick::Axis		 m_joystickAxis;
	};
}

namespace ImGui
{	
	//================================================================================================================================
	//================================================================================================================================
	bool FanAxis( const char * _label, fan::Axis* _axis );
}