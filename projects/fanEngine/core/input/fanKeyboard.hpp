#pragma once

#include <array>
#include <vector>
#include <string>
#include "core/fanSingleton.hpp"

struct GLFWwindow;

namespace fan
{
    //==================================================================================================================================================================================================
    // helper class to access keyboard input
    //==================================================================================================================================================================================================
    class Keyboard : public Singleton<Keyboard>
    {
        friend class Singleton<Keyboard>;
        friend class Input;
    public:
        using Key = int;

        static bool IsKeyDown( const Key _key );
        static bool IsKeyPressed( const Key _key );
        static bool IsKeyReleased( const Key _key );
        static std::string GetKeyName( const Key _key );
        const std::vector<Key>& GetKeysList() const { return s_keysList; }

    private:
        Keyboard();
        static void KeyCallback( GLFWwindow* _window, int _key, int _scancode, int _action, int _mods );
        static void CharCallback( GLFWwindow* _window, unsigned int _c );

        //Events
        std::vector<uint64_t> mKeysPressed;
        std::vector<uint64_t> mKeysReleased;

    public:
        static const Key SPACE;
        static const Key APOSTROPHE;
        static const Key COMMA;
        static const Key MINUS;
        static const Key PERIOD;
        static const Key SLASH;
        static const Key KEY0;
        static const Key KEY1;
        static const Key KEY2;
        static const Key KEY3;
        static const Key KEY4;
        static const Key KEY5;
        static const Key KEY6;
        static const Key KEY7;
        static const Key KEY8;
        static const Key KEY9;
        static const Key SEMICOLON;
        static const Key EQUAL;
        static const Key A;
        static const Key B;
        static const Key C;
        static const Key D;
        static const Key E;
        static const Key F;
        static const Key G;
        static const Key H;
        static const Key I;
        static const Key J;
        static const Key K;
        static const Key L;
        static const Key M;
        static const Key N;
        static const Key O;
        static const Key P;
        static const Key Q;
        static const Key R;
        static const Key S;
        static const Key T;
        static const Key U;
        static const Key V;
        static const Key W;
        static const Key X;
        static const Key Y;
        static const Key Z;
        static const Key LEFT_BRACKET;
        static const Key BACKSLASH;
        static const Key RIGHT_BRACKET;
        static const Key GRAVE_ACCENT;
        static const Key WORLD_1;
        static const Key WORLD_2;
        static const Key ESCAPE;
        static const Key ENTER;
        static const Key TAB;
        static const Key BACKSPACE;
        static const Key INSERT;
        static const Key DEL;
        static const Key RIGHT;
        static const Key LEFT;
        static const Key DOWN;
        static const Key UP;
        static const Key PAGE_UP;
        static const Key PAGE_DOWN;
        static const Key HOME;
        static const Key END;
        static const Key CAPS_LOCK;
        static const Key SCROLL_LOCK;
        static const Key NUM_LOCK;
        static const Key PRINT_SCREEN;
        static const Key PAUSE;
        static const Key F1;
        static const Key F2;
        static const Key F3;
        static const Key F4;
        static const Key F5;
        static const Key F6;
        static const Key F7;
        static const Key F8;
        static const Key F9;
        static const Key F10;
        static const Key F11;
        static const Key F12;
        static const Key F13;
        static const Key F14;
        static const Key F15;
        static const Key F16;
        static const Key F17;
        static const Key F18;
        static const Key F19;
        static const Key F20;
        static const Key F21;
        static const Key F22;
        static const Key F23;
        static const Key F24;
        static const Key F25;
        static const Key KP_0;
        static const Key KP_1;
        static const Key KP_2;
        static const Key KP_3;
        static const Key KP_4;
        static const Key KP_5;
        static const Key KP_6;
        static const Key KP_7;
        static const Key KP_8;
        static const Key KP_9;
        static const Key KP_DECIMAL;
        static const Key KP_DIVIDE;
        static const Key KP_MULTIPLY;
        static const Key KP_SUBTRACT;
        static const Key KP_ADD;
        static const Key KP_ENTER;
        static const Key KP_EQUAL;
        static const Key LEFT_SHIFT;
        static const Key LEFT_CONTROL;
        static const Key LEFT_ALT;
        static const Key LEFT_SUPER;
        static const Key RIGHT_SHIFT;
        static const Key RIGHT_CONTROL;
        static const Key RIGHT_ALT;
        static const Key RIGHT_SUPER;
        static const Key MENU;
        static const Key NONE;

        static const std::vector<const char*> keyName;
        static const std::vector<Key>   s_keysList;
    };
}