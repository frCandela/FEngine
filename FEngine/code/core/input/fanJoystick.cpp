#include "fanGlobalIncludes.h"
#include "core/input/fanJoystick.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================	
	Joystick::Joystick()
	{
		glfwSetJoystickCallback(JoystickCallback);
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Joystick::IsConnected( const int _GLFW_JOYSTICK ) const
	{
		assert( _GLFW_JOYSTICK <= GLFW_JOYSTICK_LAST );
		return glfwJoystickPresent(_GLFW_JOYSTICK) == GLFW_TRUE;
	}

	//================================================================================================================================
	//================================================================================================================================	
	int  Joystick::NumConnectedJoysticks() const
	{
		int num = 0;
		for ( int joystickIndex = 0; joystickIndex <= GLFW_JOYSTICK_LAST; joystickIndex++ )
		{
			if ( IsConnected( joystickIndex ) )
			{
				++num;
			}
		}
		return num;
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Joystick::IsGamepad( const int _GLFW_JOYSTICK ) const
	{
		return glfwJoystickIsGamepad( _GLFW_JOYSTICK );
	}

	//================================================================================================================================
	//================================================================================================================================	
	std::vector<float>  Joystick::GetAxes( const int _GLFW_JOYSTICK )
	{		
		int count;
		const float* axes = glfwGetJoystickAxes( _GLFW_JOYSTICK, &count );

		std::vector<float>  axesVec;
		axesVec.reserve( count );
		for (int axisIndex = 0; axisIndex < count ; axisIndex++)
		{
			axesVec.push_back(axes[axisIndex]);
		}
		return axesVec;
	}

	//================================================================================================================================
	//================================================================================================================================	
	std::vector<bool>  Joystick::GetButtons( const int _GLFW_JOYSTICK )
	{
		int count;
		const unsigned char* buttons = glfwGetJoystickButtons( _GLFW_JOYSTICK, &count );

		std::vector<bool>  axesVec;
		axesVec.reserve( count );
		for ( int axisIndex = 0; axisIndex < count; axisIndex++ )
		{
			axesVec.push_back( buttons[axisIndex] == GLFW_PRESS );
		}
		return axesVec;
	}

	//================================================================================================================================
	//================================================================================================================================	
	std::vector<float>	Joystick::GetHats( const int _GLFW_JOYSTICK )
	{
		int count;
		const unsigned char* hats = glfwGetJoystickHats( _GLFW_JOYSTICK, &count );

		std::vector<float>  hatsVec;
		hatsVec.reserve( count );

		for ( int axisIndex = 0; axisIndex < count; axisIndex++ )
		{
			float hatAxisX = 0.f ;
			float hatAxisY = 0.f ;			

			switch ( hats[axisIndex] )
			{
			case GLFW_HAT_CENTERED:
				break;
			case GLFW_HAT_UP:
				hatAxisY = 1.f;
				break;
			case GLFW_HAT_RIGHT:
				hatAxisX = 1.f;
				break;
			case GLFW_HAT_DOWN:
				hatAxisY = -1.f;
				break;
			case GLFW_HAT_LEFT:
				hatAxisX = -1.f;
				break;
			case GLFW_HAT_RIGHT_UP:
				hatAxisX = 1.f;
				hatAxisY = 1.f;
				break;
			case GLFW_HAT_RIGHT_DOWN:
				hatAxisX = 1.f;
				hatAxisY = -1.f;
				break;
			case GLFW_HAT_LEFT_UP:
				hatAxisX = -1.f;
				hatAxisY = 1.f;
				break;
			case GLFW_HAT_LEFT_DOWN:
				hatAxisX = -1.f;
				hatAxisY = -1.f;
				break;
			default:
				assert(false);
				break;
			}

			hatsVec.push_back( hatAxisX );
			hatsVec.push_back( hatAxisY );
		}
		return hatsVec;
	}

	//================================================================================================================================
	//================================================================================================================================	
	std::string Joystick::GetName( const int _GLFW_JOYSTICK )
	{
		return glfwGetJoystickName(_GLFW_JOYSTICK);
	}

	//================================================================================================================================
	//================================================================================================================================	
	std::string Joystick::GetGamepadName( const int _GLFW_JOYSTICK )
	{
		return IsGamepad( _GLFW_JOYSTICK ) ? glfwGetGamepadName(_GLFW_JOYSTICK):  "";
	}

	//================================================================================================================================
	//================================================================================================================================	
	float Joystick::GetAxis( const int _GLFW_JOYSTICK, const Axis _GLFW_GAMEPAD_AXIS )
	{
		GLFWgamepadstate state;
		if ( glfwGetGamepadState( _GLFW_JOYSTICK, &state ) )
		{
			return state.axes[_GLFW_GAMEPAD_AXIS];
		}
		else
		{
			return 0.f;
		}
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Joystick::GetButton( const int _GLFW_JOYSTICK, const Button _GLFW_GAMEPAD_BUTTON )
	{
		GLFWgamepadstate state;
		if ( glfwGetGamepadState( _GLFW_JOYSTICK, &state ) )
		{
			return state.buttons[_GLFW_GAMEPAD_BUTTON];
		}
		else
		{
			return  false;
		}
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Joystick::JoystickCallback( int _jid, int _event )
	{
		Joystick::Get().onJoystickConnect.Emmit(_jid,  _event == GLFW_CONNECTED);
		Debug::Log() << "Joystick " << _jid <<  (_event == GLFW_CONNECTED ? " connect" : " disconnect") << Debug::Endl();
	}

	//================================================================================================================================
	//================================================================================================================================	
	const char * Joystick::s_axisNames[GLFW_GAMEPAD_AXIS_LAST + 1] =
	{    "LEFT_X       "
		,"LEFT_Y       "
		,"RIGHT_X      "
		,"RIGHT_Y      "
		,"LEFT_TRIGGER "
		,"RIGHT_TRIGGER"
	};

	const char * Joystick::s_buttonsNames[GLFW_GAMEPAD_BUTTON_LAST + 1] = 
	{    "A           "
		,"B           "
		,"X           "
		,"Y           "
		,"LEFT_BUMPER "
		,"RIGHT_BUMPER"
		,"BACK        "
		,"START       "
		,"GUIDE       "
		,"LEFT_THUMB  "
		,"RIGHT_THUMB "
		,"DPAD_UP     "
		,"DPAD_RIGHT  "
		,"DPAD_DOWN   "
		,"DPAD_LEFT   "
	};

}