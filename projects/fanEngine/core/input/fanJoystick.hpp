#pragma once

#include "core/fanSingleton.hpp"
#include "core/fanSignal.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Helper class to access connected game pads and their inputs
    //==================================================================================================================================================================================================
    class Joystick : public Singleton<Joystick>
    {
        friend class Singleton<Joystick>;
    public:
        using Button = int;
        using Axis = int;
        using JoystickID = int;

        bool IsGamepad( const JoystickID _GLFW_JOYSTICK ) const;
        bool IsConnected( const JoystickID _GLFW_JOYSTICK ) const;
        int NumConnectedJoysticks() const;

        // Generic input
        std::vector<float> GetAxes( const JoystickID _GLFW_JOYSTICK );
        std::vector<bool> GetButtons( const JoystickID _GLFW_JOYSTICK );
        std::vector<float> GetHats( const JoystickID _GLFW_JOYSTICK );
        std::string GetName( const JoystickID _GLFW_JOYSTICK );

        // Gamepad input
        std::string GetGamepadName( const JoystickID _GLFW_JOYSTICK );
        float GetAxis( const JoystickID _GLFW_JOYSTICK, const Axis _axis );
        bool GetButton( const JoystickID _GLFW_JOYSTICK, const Button _button );
        std::string GetAxisName( const Axis _key );
        std::string GetButtonName( const Button _button );
        const std::vector<Joystick::Button>& GetGamepadButtonsList() const { return sButtonsList; };
        const std::vector<Joystick::Axis>& GetGamepadAxisList() const { return sAxesList; };

        Signal<JoystickID, bool> onJoystickConnect; // joystick id, connected/disconnected

    private:
        Joystick();

        static void JoystickCallback( int _jid, int _event );

    public:
        //Joysticks
        static const JoystickID JOYSTICK_ANY;
        static const JoystickID NUM_JOYSTICK;

        // Button
        static const Button                      BUTTON_NONE;
        static const Button                      A;
        static const Button                      B;
        static const Button                      X;
        static const Button                      Y;
        static const Button                      LEFT_BUMPER;
        static const Button                      RIGHT_BUMPER;
        static const Button                      BACK;
        static const Button                      START;
        static const Button                      GUIDE;
        static const Button                      LEFT_THUMB;
        static const Button                      RIGHT_THUMB;
        static const Button                      DPAD_UP;
        static const Button                      DPAD_RIGHT;
        static const Button                      DPAD_DOWN;
        static const Button                      DPAD_LEFT;
        static const std::vector<const char*>    sButtonsNames;
        static const std::vector<Joystick::Axis> sAxesList;

        // Axis
        static const Axis                          AXIS_NONE;
        static const Axis                          LEFT_X;
        static const Axis                          LEFT_Y;
        static const Axis                          RIGHT_X;
        static const Axis                          RIGHT_Y;
        static const Axis                          LEFT_TRIGGER;
        static const Axis                          RIGHT_TRIGGER;
        static const std::vector<const char*>      sAxisNames;
        static const std::vector<Joystick::Button> sButtonsList;
    };
}