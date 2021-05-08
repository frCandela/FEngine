#pragma once

#include "glfw/glfw3.h"
#include "core/fanSingleton.hpp"
#include "core/ecs/fanSignal.hpp"

namespace fan
{
	//========================================================================================================
	// Helper class to access connected game pads and their inputs
	//========================================================================================================
	class Joystick : public Singleton<Joystick>
	{
		friend class Singleton<Joystick>;
	public:
		using Button = int;
		using Axis = int;
		using JoystickID = int;

		bool IsGamepad( const JoystickID _GLFW_JOYSTICK ) const;
		bool IsConnected( const JoystickID _GLFW_JOYSTICK ) const;
		int  NumConnectedJoysticks() const;

		// Generic input
		std::vector<float>	GetAxes( const JoystickID _GLFW_JOYSTICK );
		std::vector<bool>	GetButtons( const JoystickID _GLFW_JOYSTICK );
		std::vector<float>	GetHats( const JoystickID _GLFW_JOYSTICK );
		std::string			GetName( const JoystickID _GLFW_JOYSTICK );

		// Gamepad input
		std::string	 GetGamepadName( const JoystickID _GLFW_JOYSTICK );
		float		 GetAxis( const JoystickID _GLFW_JOYSTICK, const Axis _axis );
		bool		 GetButton( const JoystickID _GLFW_JOYSTICK, const Button _button );
		std::string  GetAxisName( const Axis _key );
		std::string  GetButtonName( const Button _button );
		const std::vector< Joystick::Button >& GetGamepadButtonsList() const { return  sButtonsList; };
		const std::vector< Joystick::Axis >& GetGamepadAxisList() const { return  sAxesList; };

		Signal<JoystickID, bool> onJoystickConnect; // joystick id, connected/disconnected 

	private:
		Joystick();

		static void JoystickCallback( int _jid, int _event );

	public:
		//Joysticks
		static const JoystickID JOYSTICK_ANY = GLFW_JOYSTICK_LAST + 1;
		static const JoystickID NUM_JOYSTICK = GLFW_JOYSTICK_LAST + 1;

		// Button
		static const Button BUTTON_NONE = -1;
		static const Button A = GLFW_GAMEPAD_BUTTON_A;
		static const Button B = GLFW_GAMEPAD_BUTTON_B;
		static const Button X = GLFW_GAMEPAD_BUTTON_X;
		static const Button Y = GLFW_GAMEPAD_BUTTON_Y;
		static const Button LEFT_BUMPER = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
		static const Button RIGHT_BUMPER = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
		static const Button BACK = GLFW_GAMEPAD_BUTTON_BACK;
		static const Button START = GLFW_GAMEPAD_BUTTON_START;
		static const Button GUIDE = GLFW_GAMEPAD_BUTTON_GUIDE;
		static const Button LEFT_THUMB = GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
		static const Button RIGHT_THUMB = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
		static const Button DPAD_UP = GLFW_GAMEPAD_BUTTON_DPAD_UP;
		static const Button DPAD_RIGHT = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
		static const Button DPAD_DOWN = GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
		static const Button DPAD_LEFT = GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
		static const char* sButtonsNames[GLFW_GAMEPAD_BUTTON_LAST + 1 ];
		static const std::vector< Joystick::Axis > sAxesList;

		// Axis
		static const Axis AXIS_NONE = -1;
		static const Axis LEFT_X = GLFW_GAMEPAD_AXIS_LEFT_X;
		static const Axis LEFT_Y = GLFW_GAMEPAD_AXIS_LEFT_Y;
		static const Axis RIGHT_X = GLFW_GAMEPAD_AXIS_RIGHT_X;
		static const Axis RIGHT_Y = GLFW_GAMEPAD_AXIS_RIGHT_Y;
		static const Axis LEFT_TRIGGER = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;
		static const Axis RIGHT_TRIGGER = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;
		static const char* sAxisNames[GLFW_GAMEPAD_AXIS_LAST + 1 ];
		static const std::vector< Joystick::Button > sButtonsList;
	};
}