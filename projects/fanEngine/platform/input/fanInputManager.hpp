#pragma once

#include "fanJson.hpp"
#include "platform/input/fanAxis.hpp"
#include "core/fanSingleton.hpp"
#include "core/memory/fanSerializable.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // stores input axis and keyboard events.
    // @todo the input manager must DIE
    //==================================================================================================================================================================================================
    class InputManager
    {
    public:
        //================================================================
        // KeyboardEvent is a keyboard shortcut composed of a main key and 3 facultative modifiers keys
        //================================================================
        struct KeyboardEvent
        {
            Keyboard::Key key;
            Keyboard::Key mod0;
            Keyboard::Key mod1;
            Keyboard::Key mod2;
            Signal<>      onEvent;
        };

        //================================================================
        //================================================================
        void CreateKeyboardAxis( const std::string& _name, const Keyboard::Key _keyPositive, const Keyboard::Key _keyNegative );
        void CreateJoystickAxis( const std::string& _name, const Joystick::JoystickID _GLFW_JOYSTICK, const Joystick::Axis _axis );
        void CreateJoystickButtons( const std::string& _name, const Joystick::JoystickID _GLFW_JOYSTICK, const Joystick::Button _positive, const Joystick::Button _negative = Joystick::BUTTON_NONE );
        Signal<>* CreateKeyboardEvent( const std::string& _name,
                                       const Keyboard::Key _key,
                                       const Keyboard::Key _mod0 = Keyboard::NONE,
                                       const Keyboard::Key _mod1 = Keyboard::NONE,
                                       const Keyboard::Key _mod2 = Keyboard::NONE );
        Signal<>* FindEvent( const std::string& _name );
        float GetAxis( const std::string& _name, const int _joystickIDOverride = -1 );

        std::map<std::string, Axis>& GetListAxis() { return mAxis; }
        std::map<std::string, KeyboardEvent>& GetListKeyboardEvents() { return mKeyboardEvents; }

        void PullEvents();
        bool Load( const Json& _json );
        bool Save( Json& _json ) const;

    private:
        std::map<std::string, Axis>          mAxis;
        std::map<std::string, KeyboardEvent> mKeyboardEvents;
    };
}