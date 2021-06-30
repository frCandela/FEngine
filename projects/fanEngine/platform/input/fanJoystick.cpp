#include "platform/input/fanJoystick.hpp"
#include <windows.h>
#include "GLFW/glfw3.h"
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
    const std::vector<const char*> Joystick::sAxisNames =
                      {
                              "LEFT_X         ",
                              "LEFT_Y         ",
                              "RIGHT_X        ",
                              "RIGHT_Y        ",
                              "LEFT_TRIGGER   ",
                              "RIGHT_TRIGGER  ",
                      };

    const std::vector<const char*> Joystick::sButtonsNames=
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

    const Joystick::JoystickID Joystick::JOYSTICK_ANY = GLFW_JOYSTICK_LAST + 1;
    const Joystick::JoystickID Joystick::NUM_JOYSTICK = GLFW_JOYSTICK_LAST + 1;

    // Button
    const Joystick::Button Joystick::BUTTON_NONE  = -1;
    const Joystick::Button Joystick::A            = GLFW_GAMEPAD_BUTTON_A;
    const Joystick::Button Joystick::B            = GLFW_GAMEPAD_BUTTON_B;
    const Joystick::Button Joystick::X            = GLFW_GAMEPAD_BUTTON_X;
    const Joystick::Button Joystick::Y            = GLFW_GAMEPAD_BUTTON_Y;
    const Joystick::Button Joystick::LEFT_BUMPER  = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
    const Joystick::Button Joystick::RIGHT_BUMPER = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
    const Joystick::Button Joystick::BACK         = GLFW_GAMEPAD_BUTTON_BACK;
    const Joystick::Button Joystick::START        = GLFW_GAMEPAD_BUTTON_START;
    const Joystick::Button Joystick::GUIDE        = GLFW_GAMEPAD_BUTTON_GUIDE;
    const Joystick::Button Joystick::LEFT_THUMB   = GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
    const Joystick::Button Joystick::RIGHT_THUMB  = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
    const Joystick::Button Joystick::DPAD_UP      = GLFW_GAMEPAD_BUTTON_DPAD_UP;
    const Joystick::Button Joystick::DPAD_RIGHT   = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
    const Joystick::Button Joystick::DPAD_DOWN    = GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
    const Joystick::Button Joystick::DPAD_LEFT    = GLFW_GAMEPAD_BUTTON_DPAD_LEFT;

    // Axis
    const Joystick::Axis Joystick::AXIS_NONE     = -1;
    const Joystick::Axis Joystick::LEFT_X        = GLFW_GAMEPAD_AXIS_LEFT_X;
    const Joystick::Axis Joystick::LEFT_Y        = GLFW_GAMEPAD_AXIS_LEFT_Y;
    const Joystick::Axis Joystick::RIGHT_X       = GLFW_GAMEPAD_AXIS_RIGHT_X;
    const Joystick::Axis Joystick::RIGHT_Y       = GLFW_GAMEPAD_AXIS_RIGHT_Y;
    const Joystick::Axis Joystick::LEFT_TRIGGER  = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;
    const Joystick::Axis Joystick::RIGHT_TRIGGER = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;
}