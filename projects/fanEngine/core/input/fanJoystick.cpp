#include "core/input/fanJoystick.hpp"
#include "core/fanDebug.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Joystick::Joystick()
    {
        glfwSetJoystickCallback( JoystickCallback );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Joystick::IsConnected( const JoystickID _GLFW_JOYSTICK ) const
    {
        fanAssert( _GLFW_JOYSTICK <= NUM_JOYSTICK );
        return glfwJoystickPresent( _GLFW_JOYSTICK ) == GLFW_TRUE;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    int Joystick::NumConnectedJoysticks() const
    {
        int      num           = 0;
        for( int joystickIndex = 0; joystickIndex < NUM_JOYSTICK; joystickIndex++ )
        {
            if( IsConnected( joystickIndex ) )
            {
                ++num;
            }
        }
        return num;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Joystick::IsGamepad( const JoystickID _GLFW_JOYSTICK ) const
    {
        return glfwJoystickIsGamepad( _GLFW_JOYSTICK );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<float> Joystick::GetAxes( const JoystickID _GLFW_JOYSTICK )
    {
        int count;
        const float* axes = glfwGetJoystickAxes( _GLFW_JOYSTICK, &count );

        std::vector<float> axesVec;
        axesVec.reserve( count );
        for( int axisIndex = 0; axisIndex < count; axisIndex++ )
        {
            axesVec.push_back( axes[axisIndex] );
        }
        return axesVec;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<bool> Joystick::GetButtons( const JoystickID _GLFW_JOYSTICK )
    {
        int count;
        const unsigned char* buttons = glfwGetJoystickButtons( _GLFW_JOYSTICK, &count );

        std::vector<bool> axesVec;
        axesVec.reserve( count );
        for( int axisIndex = 0; axisIndex < count; axisIndex++ )
        {
            axesVec.push_back( buttons[axisIndex] == GLFW_PRESS );
        }
        return axesVec;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<float> Joystick::GetHats( const JoystickID _GLFW_JOYSTICK )
    {
        int count;
        const unsigned char* hats = glfwGetJoystickHats( _GLFW_JOYSTICK, &count );

        std::vector<float> hatsVec;
        hatsVec.reserve( count );

        for( int axisIndex = 0; axisIndex < count; axisIndex++ )
        {
            float hatAxisX = 0.f;
            float hatAxisY = 0.f;

            switch( hats[axisIndex] )
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
                    fanAssert( false );
                    break;
            }

            hatsVec.push_back( hatAxisX );
            hatsVec.push_back( hatAxisY );
        }
        return hatsVec;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::string Joystick::GetName( const JoystickID _GLFW_JOYSTICK )
    {
        return glfwGetJoystickName( _GLFW_JOYSTICK );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::string Joystick::GetGamepadName( const JoystickID _GLFW_JOYSTICK )
    {
        return IsGamepad( _GLFW_JOYSTICK ) ? glfwGetGamepadName( _GLFW_JOYSTICK ) : "";
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    float Joystick::GetAxis( const JoystickID _GLFW_JOYSTICK, const Axis _GLFW_GAMEPAD_AXIS )
    {
        GLFWgamepadstate state;
        if( _GLFW_JOYSTICK < 0 ||
            _GLFW_GAMEPAD_AXIS == AXIS_NONE ||
            !glfwGetGamepadState( _GLFW_JOYSTICK, &state ) )
        {
            return 0.f;
        }
        else
        {
            return state.axes[_GLFW_GAMEPAD_AXIS];
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Joystick::GetButton( const JoystickID _GLFW_JOYSTICK, const Button _GLFW_GAMEPAD_BUTTON )
    {
        GLFWgamepadstate state;
        if( _GLFW_JOYSTICK >= 0 &&
            _GLFW_GAMEPAD_BUTTON >= 0 &&
            glfwGetGamepadState( _GLFW_JOYSTICK, &state ) )
        {
            return state.buttons[_GLFW_GAMEPAD_BUTTON];
        }
        else
        {
            return false;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::string Joystick::GetAxisName( const Joystick::Axis _key )
    {
        if( _key >= 0 ){ return sAxisNames[_key]; }
        else{ return "               "; }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::string Joystick::GetButtonName( const Button _button )
    {
        if( _button >= 0 ){ return sButtonsNames[_button]; }
        else{ return "               "; }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Joystick::JoystickCallback( int _jid, int _event )
    {
        Joystick::Get().onJoystickConnect.Emmit( _jid, _event == GLFW_CONNECTED );
        Debug::Log() << "Joystick " << _jid
                << ( _event == GLFW_CONNECTED ? " connect" : " disconnect" ) << Debug::Endl();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    const char* Joystick::sAxisNames[GLFW_GAMEPAD_AXIS_LAST + 1] =
                      {
                              "LEFT_X         ",
                              "LEFT_Y         ",
                              "RIGHT_X        ",
                              "RIGHT_Y        ",
                              "LEFT_TRIGGER   ",
                              "RIGHT_TRIGGER  ",
                      };

    const char* Joystick::sButtonsNames[GLFW_GAMEPAD_BUTTON_LAST + 1] =
                      {
                              "A              ",
                              "B              ",
                              "X              ",
                              "Y              ",
                              "LEFT_BUMPER    ",
                              "RIGHT_BUMPER   ",
                              "BACK           ",
                              "START          ",
                              "GUIDE          ",
                              "LEFT_THUMB     ",
                              "RIGHT_THUMB    ",
                              "DPAD_UP        ",
                              "DPAD_RIGHT     ",
                              "DPAD_DOWN      ",
                              "DPAD_LEFT      ",
                      };

    const std::vector<Joystick::Axis> Joystick::sAxesList =
                                              {
                                                      Joystick::LEFT_X,
                                                      Joystick::LEFT_Y,
                                                      Joystick::RIGHT_X,
                                                      Joystick::RIGHT_Y,
                                                      Joystick::LEFT_TRIGGER,
                                                      Joystick::RIGHT_TRIGGER,
                                              };

    const std::vector<Joystick::Button> Joystick::sButtonsList =
                                                {
                                                        Joystick::A,
                                                        Joystick::B,
                                                        Joystick::X,
                                                        Joystick::Y,
                                                        Joystick::LEFT_BUMPER,
                                                        Joystick::RIGHT_BUMPER,
                                                        Joystick::BACK,
                                                        Joystick::START,
                                                        Joystick::GUIDE,
                                                        Joystick::LEFT_THUMB,
                                                        Joystick::RIGHT_THUMB,
                                                        Joystick::DPAD_UP,
                                                        Joystick::DPAD_RIGHT,
                                                        Joystick::DPAD_DOWN,
                                                        Joystick::DPAD_LEFT,
                                                };
}