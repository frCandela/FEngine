#include "core/input/fanKeyboard.hpp"

#include <cctype>
#include "fanWindowsH.hpp"
#include "glfw/glfw3.h"
#include "imgui/imgui.h"
#include "core/input/fanInput.hpp"
#include "core/fanAssert.hpp"

#undef max

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Keyboard::Keyboard()
    {
        mKeysPressed.resize(GLFW_KEY_LAST + 1);
        mKeysReleased.resize(GLFW_KEY_LAST + 1);
        fanAssert( mKeysPressed.size() == mKeysReleased.size() );
        const uint64_t max         = std::numeric_limits<uint64_t>::max();
        for( int       buttonIndex = 0; buttonIndex < (int)mKeysPressed.size(); buttonIndex++ )
        {
            mKeysPressed[buttonIndex]  = max;
            mKeysReleased[buttonIndex] = max;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Keyboard::IsKeyDown( const Key _key )
    {
        if( _key == NONE || ImGui::GetIO().WantCaptureKeyboard )
        {
            return false;
        }
        else
        {
            return glfwGetKey( Input::Get().Window(), _key ) == GLFW_PRESS;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Keyboard::IsKeyPressed( const Key _key )
    {
        if( _key == NONE || ImGui::GetIO().WantCaptureKeyboard )
        {
            return false;
        }
        else
        {
            return Get().mKeysPressed[_key] == Input::Get().FrameCount();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Keyboard::IsKeyReleased( const Key _key )
    {
        if( _key == NONE || ImGui::GetIO().WantCaptureKeyboard )
        {
            return false;
        }
        else
        {
            return Get().mKeysReleased[_key] == Input::Get().FrameCount();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::string Keyboard::GetKeyName( const Key _key )
    {
        const char* glfwName = glfwGetKeyName( _key, 0 );
        if( glfwName == nullptr )
        {
            return keyName[_key];
        }
        else
        {
            std::string name( glfwName );

            for( int charIndex = 0; charIndex < (int)name.size(); charIndex++ )
            {
                name[charIndex] = (char)std::toupper( name[charIndex] );
            }
            name.resize( 15, ' ' );

            return name;
        }
    }

    //==================================================================================================================================================================================================
    // Modifiers are not reliable across systems
    //==================================================================================================================================================================================================
    void Keyboard::KeyCallback( GLFWwindow* /*_window*/, int _key, int /*_scancode*/, int _action, int                                /*_mods*/ )
    {
        if( _key == GLFW_KEY_UNKNOWN ){ return; }

        ImGuiIO& io = ImGui::GetIO();

        // Dirty hack so that pressing KP_ENTER is considered by IMGUI
        if( _key == GLFW_KEY_KP_ENTER )
        {
            _key = GLFW_KEY_ENTER;
        }

        //Imgui
        if( _action == GLFW_PRESS )
        {
            io.KeysDown[_key] = true;
        }
        if( _action == GLFW_RELEASE )
        {
            io.KeysDown[_key] = false;
        }

        io.KeyCtrl  = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
        io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
        io.KeyAlt   = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
        io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

        if( _action == GLFW_PRESS )
        {
            Get().mKeysPressed[_key] = Input::Get().FrameCount();
        }
        else if( _action == GLFW_RELEASE )
        {
            Get().mKeysReleased[_key] = Input::Get().FrameCount();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Keyboard::CharCallback( GLFWwindow* /*_window*/, unsigned int _c )
    {
        ImGuiIO& io = ImGui::GetIO();
        if( _c > 0 && _c < 0x10000 )
        {
            io.AddInputCharacter( (unsigned short)_c );
        }
    }

    const std::vector<const char *> Keyboard::keyName = {
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "SPACE          "/*32*/        ,
            "",
            "",
            "",
            "",
            "",
            "",
            "APOSTROPHE     "/*39*/        ,
            "",
            "",
            "",
            "",
            "COMMA          "/*44*/
            ,
            "MINUS          ",
            "PERIOD         ",
            "SLASH          ",
            "KEY0           ",
            "KEY1           ",
            "KEY2           "/*50*/
            ,
            "KEY3           ",
            "KEY4           ",
            "KEY5           ",
            "KEY6           ",
            "KEY7           ",
            "KEY8           ",
            "KEY9           "/*57*/,
            "",
            "SEMICOLON      "/*59*/,
            "",
            "EQUAL          "/*61*/,
            "",
            "",
            "",
            "A              "/*65*/
            ,
            "B              ",
            "C              ",
            "D              ",
            "E              ",
            "F              "/*70*/
            ,
            "G              ",
            "H              ",
            "I              ",
            "J              ",
            "K              ",
            "L              ",
            "M              ",
            "N              ",
            "O              ",
            "P              "/*80*/
            ,
            "Q              ",
            "R              ",
            "S              ",
            "T              ",
            "U              ",
            "V              ",
            "W              ",
            "X              ",
            "Y              ",
            "Z              "/*90*/
            ,
            "LEFT_BRACKET   ",
            "BACKSLASH      ",
            "RIGHT_BRACKET  "/*93*/,
            "",
            "",
            "GRAVE_ACCENT   "/*96*/,
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "WORLD_1        "/*161*/
            ,
            "WORLD_2        "/*162*/,
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "ESCAPE         "/*256*/
            ,
            "ENTER          ",
            "TAB            ",
            "BACKSPACE      ",
            "INSERT         "/*260*/
            ,
            "DELETE         ",
            "RIGHT          ",
            "LEFT           ",
            "DOWN           ",
            "UP             ",
            "PAGE_UP        ",
            "PAGE_DOWN      ",
            "HOME           ",
            "END            "/*269*/,
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "CAPS_LOCK      "/*280*/
            ,
            "SCROLL_LOCK    ",
            "NUM_LOCK       ",
            "PRINT_SCREEN   ",
            "PAUSE          "/*284*/,
            "",
            "",
            "",
            "",
            "",
            "F1             "/*290*/
            ,
            "F2             ",
            "F3             ",
            "F4             ",
            "F5             ",
            "F6             ",
            "F7             ",
            "F8             ",
            "F9             ",
            "F10            ",
            "F11            "/*300*/
            ,
            "F12            ",
            "F13            ",
            "F14            ",
            "F15            ",
            "F16            ",
            "F17            ",
            "F18            ",
            "F19            ",
            "F20            ",
            "F21            "/*310*/
            ,
            "F22            ",
            "F23            ",
            "F24            ",
            "F25            "/*314*/,
            "",
            "",
            "",
            "",
            "",
            "KP_0           "/*320*/
            ,
            "KP_1           ",
            "KP_2           ",
            "KP_3           ",
            "KP_4           ",
            "KP_5           ",
            "KP_6           ",
            "KP_7           ",
            "KP_8           ",
            "KP_9           ",
            "KP_DECIMAL     "/*330*/
            ,
            "KP_DIVIDE      ",
            "KP_MULTIPLY    ",
            "KP_SUBTRACT    ",
            "KP_ADD         ",
            "KP_ENTER       ",
            "KP_EQUAL       "/*336*/,
            "",
            "",
            "",
            "LEFT_SHIFT     "/*340*/
            ,
            "LEFT_CONTROL   ",
            "LEFT_ALT       ",
            "LEFT_SUPER     ",
            "RIGHT_SHIFT    ",
            "RIGHT_CONTROL  ",
            "RIGHT_ALT      ",
            "RIGHT_SUPER    ",
            "MENU           "/*348*/
            ,
            "               "/*349*/
    };

    const Keyboard::Key Keyboard::SPACE         = GLFW_KEY_SPACE;
    const Keyboard::Key Keyboard::APOSTROPHE    = GLFW_KEY_APOSTROPHE;
    const Keyboard::Key Keyboard::COMMA         = GLFW_KEY_COMMA;
    const Keyboard::Key Keyboard::MINUS         = GLFW_KEY_MINUS;
    const Keyboard::Key Keyboard::PERIOD        = GLFW_KEY_PERIOD;
    const Keyboard::Key Keyboard::SLASH         = GLFW_KEY_SLASH;
    const Keyboard::Key Keyboard::KEY0          = GLFW_KEY_0;
    const Keyboard::Key Keyboard::KEY1          = GLFW_KEY_1;
    const Keyboard::Key Keyboard::KEY2          = GLFW_KEY_2;
    const Keyboard::Key Keyboard::KEY3          = GLFW_KEY_3;
    const Keyboard::Key Keyboard::KEY4          = GLFW_KEY_4;
    const Keyboard::Key Keyboard::KEY5          = GLFW_KEY_5;
    const Keyboard::Key Keyboard::KEY6          = GLFW_KEY_6;
    const Keyboard::Key Keyboard::KEY7          = GLFW_KEY_7;
    const Keyboard::Key Keyboard::KEY8          = GLFW_KEY_8;
    const Keyboard::Key Keyboard::KEY9          = GLFW_KEY_9;
    const Keyboard::Key Keyboard::SEMICOLON     = GLFW_KEY_SEMICOLON;
    const Keyboard::Key Keyboard::EQUAL         = GLFW_KEY_EQUAL;
    const Keyboard::Key Keyboard::A             = GLFW_KEY_A;
    const Keyboard::Key Keyboard::B             = GLFW_KEY_B;
    const Keyboard::Key Keyboard::C             = GLFW_KEY_C;
    const Keyboard::Key Keyboard::D             = GLFW_KEY_D;
    const Keyboard::Key Keyboard::E             = GLFW_KEY_E;
    const Keyboard::Key Keyboard::F             = GLFW_KEY_F;
    const Keyboard::Key Keyboard::G             = GLFW_KEY_G;
    const Keyboard::Key Keyboard::H             = GLFW_KEY_H;
    const Keyboard::Key Keyboard::I             = GLFW_KEY_I;
    const Keyboard::Key Keyboard::J             = GLFW_KEY_J;
    const Keyboard::Key Keyboard::K             = GLFW_KEY_K;
    const Keyboard::Key Keyboard::L             = GLFW_KEY_L;
    const Keyboard::Key Keyboard::M             = GLFW_KEY_M;
    const Keyboard::Key Keyboard::N             = GLFW_KEY_N;
    const Keyboard::Key Keyboard::O             = GLFW_KEY_O;
    const Keyboard::Key Keyboard::P             = GLFW_KEY_P;
    const Keyboard::Key Keyboard::Q             = GLFW_KEY_Q;
    const Keyboard::Key Keyboard::R             = GLFW_KEY_R;
    const Keyboard::Key Keyboard::S             = GLFW_KEY_S;
    const Keyboard::Key Keyboard::T             = GLFW_KEY_T;
    const Keyboard::Key Keyboard::U             = GLFW_KEY_U;
    const Keyboard::Key Keyboard::V             = GLFW_KEY_V;
    const Keyboard::Key Keyboard::W             = GLFW_KEY_W;
    const Keyboard::Key Keyboard::X             = GLFW_KEY_X;
    const Keyboard::Key Keyboard::Y             = GLFW_KEY_Y;
    const Keyboard::Key Keyboard::Z             = GLFW_KEY_Z;
    const Keyboard::Key Keyboard::LEFT_BRACKET  = GLFW_KEY_LEFT_BRACKET;
    const Keyboard::Key Keyboard::BACKSLASH     = GLFW_KEY_BACKSLASH;
    const Keyboard::Key Keyboard::RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET;
    const Keyboard::Key Keyboard::GRAVE_ACCENT  = GLFW_KEY_GRAVE_ACCENT;
    const Keyboard::Key Keyboard::WORLD_1       = GLFW_KEY_WORLD_1;
    const Keyboard::Key Keyboard::WORLD_2       = GLFW_KEY_WORLD_2;
    const Keyboard::Key Keyboard::ESCAPE        = GLFW_KEY_ESCAPE;
    const Keyboard::Key Keyboard::ENTER         = GLFW_KEY_ENTER;
    const Keyboard::Key Keyboard::TAB           = GLFW_KEY_TAB;
    const Keyboard::Key Keyboard::BACKSPACE     = GLFW_KEY_BACKSPACE;
    const Keyboard::Key Keyboard::INSERT        = GLFW_KEY_INSERT;
    const Keyboard::Key Keyboard::DEL           = GLFW_KEY_DELETE;
    const Keyboard::Key Keyboard::RIGHT         = GLFW_KEY_RIGHT;
    const Keyboard::Key Keyboard::LEFT          = GLFW_KEY_LEFT;
    const Keyboard::Key Keyboard::DOWN          = GLFW_KEY_DOWN;
    const Keyboard::Key Keyboard::UP            = GLFW_KEY_UP;
    const Keyboard::Key Keyboard::PAGE_UP       = GLFW_KEY_PAGE_UP;
    const Keyboard::Key Keyboard::PAGE_DOWN     = GLFW_KEY_PAGE_DOWN;
    const Keyboard::Key Keyboard::HOME          = GLFW_KEY_HOME;
    const Keyboard::Key Keyboard::END           = GLFW_KEY_END;
    const Keyboard::Key Keyboard::CAPS_LOCK     = GLFW_KEY_CAPS_LOCK;
    const Keyboard::Key Keyboard::SCROLL_LOCK   = GLFW_KEY_SCROLL_LOCK;
    const Keyboard::Key Keyboard::NUM_LOCK      = GLFW_KEY_NUM_LOCK;
    const Keyboard::Key Keyboard::PRINT_SCREEN  = GLFW_KEY_PRINT_SCREEN;
    const Keyboard::Key Keyboard::PAUSE         = GLFW_KEY_PAUSE;
    const Keyboard::Key Keyboard::F1            = GLFW_KEY_F1;
    const Keyboard::Key Keyboard::F2            = GLFW_KEY_F2;
    const Keyboard::Key Keyboard::F3            = GLFW_KEY_F3;
    const Keyboard::Key Keyboard::F4            = GLFW_KEY_F4;
    const Keyboard::Key Keyboard::F5            = GLFW_KEY_F5;
    const Keyboard::Key Keyboard::F6            = GLFW_KEY_F6;
    const Keyboard::Key Keyboard::F7            = GLFW_KEY_F7;
    const Keyboard::Key Keyboard::F8            = GLFW_KEY_F8;
    const Keyboard::Key Keyboard::F9            = GLFW_KEY_F9;
    const Keyboard::Key Keyboard::F10           = GLFW_KEY_F10;
    const Keyboard::Key Keyboard::F11           = GLFW_KEY_F11;
    const Keyboard::Key Keyboard::F12           = GLFW_KEY_F12;
    const Keyboard::Key Keyboard::F13           = GLFW_KEY_F13;
    const Keyboard::Key Keyboard::F14           = GLFW_KEY_F14;
    const Keyboard::Key Keyboard::F15           = GLFW_KEY_F15;
    const Keyboard::Key Keyboard::F16           = GLFW_KEY_F16;
    const Keyboard::Key Keyboard::F17           = GLFW_KEY_F17;
    const Keyboard::Key Keyboard::F18           = GLFW_KEY_F18;
    const Keyboard::Key Keyboard::F19           = GLFW_KEY_F19;
    const Keyboard::Key Keyboard::F20           = GLFW_KEY_F20;
    const Keyboard::Key Keyboard::F21           = GLFW_KEY_F21;
    const Keyboard::Key Keyboard::F22           = GLFW_KEY_F22;
    const Keyboard::Key Keyboard::F23           = GLFW_KEY_F23;
    const Keyboard::Key Keyboard::F24           = GLFW_KEY_F24;
    const Keyboard::Key Keyboard::F25           = GLFW_KEY_F25;
    const Keyboard::Key Keyboard::KP_0          = GLFW_KEY_KP_0;
    const Keyboard::Key Keyboard::KP_1          = GLFW_KEY_KP_1;
    const Keyboard::Key Keyboard::KP_2          = GLFW_KEY_KP_2;
    const Keyboard::Key Keyboard::KP_3          = GLFW_KEY_KP_3;
    const Keyboard::Key Keyboard::KP_4          = GLFW_KEY_KP_4;
    const Keyboard::Key Keyboard::KP_5          = GLFW_KEY_KP_5;
    const Keyboard::Key Keyboard::KP_6          = GLFW_KEY_KP_6;
    const Keyboard::Key Keyboard::KP_7          = GLFW_KEY_KP_7;
    const Keyboard::Key Keyboard::KP_8          = GLFW_KEY_KP_8;
    const Keyboard::Key Keyboard::KP_9          = GLFW_KEY_KP_9;
    const Keyboard::Key Keyboard::KP_DECIMAL    = GLFW_KEY_KP_DECIMAL;
    const Keyboard::Key Keyboard::KP_DIVIDE     = GLFW_KEY_KP_DIVIDE;
    const Keyboard::Key Keyboard::KP_MULTIPLY   = GLFW_KEY_KP_MULTIPLY;
    const Keyboard::Key Keyboard::KP_SUBTRACT   = GLFW_KEY_KP_SUBTRACT;
    const Keyboard::Key Keyboard::KP_ADD        = GLFW_KEY_KP_ADD;
    const Keyboard::Key Keyboard::KP_ENTER      = GLFW_KEY_KP_ENTER;
    const Keyboard::Key Keyboard::KP_EQUAL      = GLFW_KEY_KP_EQUAL;
    const Keyboard::Key Keyboard::LEFT_SHIFT    = GLFW_KEY_LEFT_SHIFT;
    const Keyboard::Key Keyboard::LEFT_CONTROL  = GLFW_KEY_LEFT_CONTROL;
    const Keyboard::Key Keyboard::LEFT_ALT      = GLFW_KEY_LEFT_ALT;
    const Keyboard::Key Keyboard::LEFT_SUPER    = GLFW_KEY_LEFT_SUPER;
    const Keyboard::Key Keyboard::RIGHT_SHIFT   = GLFW_KEY_RIGHT_SHIFT;
    const Keyboard::Key Keyboard::RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL;
    const Keyboard::Key Keyboard::RIGHT_ALT     = GLFW_KEY_RIGHT_ALT;
    const Keyboard::Key Keyboard::RIGHT_SUPER   = GLFW_KEY_RIGHT_SUPER;
    const Keyboard::Key Keyboard::MENU          = GLFW_KEY_MENU;
    const Keyboard::Key Keyboard::NONE          = GLFW_KEY_MENU + 1;

    const std::vector<Keyboard::Key> Keyboard::s_keysList =
                                             { SPACE,
                                               APOSTROPHE,
                                               COMMA,
                                               MINUS,
                                               PERIOD,
                                               SLASH,
                                               KEY0,
                                               KEY1,
                                               KEY2,
                                               KEY3,
                                               KEY4,
                                               KEY5,
                                               KEY6,
                                               KEY7,
                                               KEY8,
                                               KEY9,
                                               SEMICOLON,
                                               EQUAL,
                                               A,
                                               B,
                                               C,
                                               D,
                                               E,
                                               F,
                                               G,
                                               H,
                                               I,
                                               J,
                                               K,
                                               L,
                                               M,
                                               N,
                                               O,
                                               P,
                                               Q,
                                               R,
                                               S,
                                               T,
                                               U,
                                               V,
                                               W,
                                               X,
                                               Y,
                                               Z,
                                               LEFT_BRACKET,
                                               BACKSLASH,
                                               RIGHT_BRACKET,
                                               GRAVE_ACCENT,
                                               WORLD_1,
                                               WORLD_2,
                                               ESCAPE,
                                               ENTER,
                                               TAB,
                                               BACKSPACE,
                                               INSERT,
                                               DEL,
                                               RIGHT,
                                               LEFT,
                                               DOWN,
                                               UP,
                                               PAGE_UP,
                                               PAGE_DOWN,
                                               HOME,
                                               END,
                                               CAPS_LOCK,
                                               SCROLL_LOCK,
                                               NUM_LOCK,
                                               PRINT_SCREEN,
                                               PAUSE,
                                               F1,
                                               F2,
                                               F3,
                                               F4,
                                               F5,
                                               F6,
                                               F7,
                                               F8,
                                               F9,
                                               F10,
                                               F11,
                                               F12,
                                               F13,
                                               F14,
                                               F15,
                                               F16,
                                               F17,
                                               F18,
                                               F19,
                                               F20,
                                               F21,
                                               F22,
                                               F23,
                                               F24,
                                               F25,
                                               KP_0,
                                               KP_1,
                                               KP_2,
                                               KP_3,
                                               KP_4,
                                               KP_5,
                                               KP_6,
                                               KP_7,
                                               KP_8,
                                               KP_9,
                                               KP_DECIMAL,
                                               KP_DIVIDE,
                                               KP_MULTIPLY,
                                               KP_SUBTRACT,
                                               KP_ADD,
                                               KP_ENTER,
                                               KP_EQUAL,
                                               LEFT_SHIFT,
                                               LEFT_CONTROL,
                                               LEFT_ALT,
                                               LEFT_SUPER,
                                               RIGHT_SHIFT,
                                               RIGHT_CONTROL,
                                               RIGHT_ALT,
                                               RIGHT_SUPER,
                                               MENU
                                             };
}