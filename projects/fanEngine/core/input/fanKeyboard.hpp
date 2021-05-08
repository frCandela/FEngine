#pragma once

#include <array>
#include <vector>
#include <string>
#include "glfw/glfw3.h"
#include "core/fanSingleton.hpp"

namespace fan
{
	//========================================================================================================
	// helper class to access keyboard input
	//========================================================================================================
	class Keyboard : public Singleton<Keyboard>
	{
		friend class Singleton < Keyboard >;
		friend class Input;
	public:
		using Key = int;

		static bool	IsKeyDown( const Key _key );
		static bool IsKeyPressed( const Key _key );
		static bool IsKeyReleased( const Key _key );
		static std::string GetKeyName( const Key _key );
		const std::vector<Key>& GetKeysList() const { return s_keysList; }

	private:
		Keyboard();
		static void KeyCallback( GLFWwindow* _window, int _key, int _scancode, int _action, int _mods );
		static void CharCallback( GLFWwindow* _window, unsigned int _c );

		//Events
		std::array< uint64_t, GLFW_KEY_LAST + 1 > mKeysPressed;
		std::array< uint64_t, GLFW_KEY_LAST + 1 > mKeysReleased;

	public:	
		static const Key SPACE			= GLFW_KEY_SPACE;
		static const Key APOSTROPHE		= GLFW_KEY_APOSTROPHE;
		static const Key COMMA			= GLFW_KEY_COMMA;
		static const Key MINUS			= GLFW_KEY_MINUS;
		static const Key PERIOD			= GLFW_KEY_PERIOD;
		static const Key SLASH			= GLFW_KEY_SLASH;
		static const Key KEY0			= GLFW_KEY_0;
		static const Key KEY1			= GLFW_KEY_1;
		static const Key KEY2			= GLFW_KEY_2;
		static const Key KEY3			= GLFW_KEY_3;
		static const Key KEY4			= GLFW_KEY_4;
		static const Key KEY5			= GLFW_KEY_5;
		static const Key KEY6			= GLFW_KEY_6;
		static const Key KEY7			= GLFW_KEY_7;
		static const Key KEY8			= GLFW_KEY_8;
		static const Key KEY9			= GLFW_KEY_9;
		static const Key SEMICOLON		= GLFW_KEY_SEMICOLON;
		static const Key EQUAL			= GLFW_KEY_EQUAL;
		static const Key A				= GLFW_KEY_A;
		static const Key B				= GLFW_KEY_B;
		static const Key C				= GLFW_KEY_C;
		static const Key D				= GLFW_KEY_D;
		static const Key E				= GLFW_KEY_E;
		static const Key F				= GLFW_KEY_F;
		static const Key G				= GLFW_KEY_G;
		static const Key H				= GLFW_KEY_H;
		static const Key I				= GLFW_KEY_I;
		static const Key J				= GLFW_KEY_J;
		static const Key K				= GLFW_KEY_K;
		static const Key L				= GLFW_KEY_L;
		static const Key M				= GLFW_KEY_M;
		static const Key N				= GLFW_KEY_N;
		static const Key O				= GLFW_KEY_O;
		static const Key P				= GLFW_KEY_P;
		static const Key Q				= GLFW_KEY_Q;
		static const Key R				= GLFW_KEY_R;
		static const Key S				= GLFW_KEY_S;
		static const Key T				= GLFW_KEY_T;
		static const Key U				= GLFW_KEY_U;
		static const Key V				= GLFW_KEY_V;
		static const Key W				= GLFW_KEY_W;
		static const Key X				= GLFW_KEY_X;
		static const Key Y				= GLFW_KEY_Y;
		static const Key Z				= GLFW_KEY_Z;
		static const Key LEFT_BRACKET	= GLFW_KEY_LEFT_BRACKET;
		static const Key BACKSLASH		= GLFW_KEY_BACKSLASH;
		static const Key RIGHT_BRACKET	= GLFW_KEY_RIGHT_BRACKET;
		static const Key GRAVE_ACCENT	= GLFW_KEY_GRAVE_ACCENT;
		static const Key WORLD_1		= GLFW_KEY_WORLD_1;
		static const Key WORLD_2		= GLFW_KEY_WORLD_2;
		static const Key ESCAPE			= GLFW_KEY_ESCAPE;
		static const Key ENTER			= GLFW_KEY_ENTER;
		static const Key TAB			= GLFW_KEY_TAB;
		static const Key BACKSPACE		= GLFW_KEY_BACKSPACE;
		static const Key INSERT			= GLFW_KEY_INSERT;
		static const Key DELETE			= GLFW_KEY_DELETE;
		static const Key RIGHT			= GLFW_KEY_RIGHT;
		static const Key LEFT			= GLFW_KEY_LEFT;
		static const Key DOWN			= GLFW_KEY_DOWN;
		static const Key UP				= GLFW_KEY_UP;
		static const Key PAGE_UP		= GLFW_KEY_PAGE_UP;
		static const Key PAGE_DOWN		= GLFW_KEY_PAGE_DOWN;
		static const Key HOME			= GLFW_KEY_HOME;
		static const Key END			= GLFW_KEY_END;
		static const Key CAPS_LOCK		= GLFW_KEY_CAPS_LOCK;
		static const Key SCROLL_LOCK	= GLFW_KEY_SCROLL_LOCK;
		static const Key NUM_LOCK		= GLFW_KEY_NUM_LOCK;
		static const Key PRINT_SCREEN	= GLFW_KEY_PRINT_SCREEN;
		static const Key PAUSE			= GLFW_KEY_PAUSE;
		static const Key F1				= GLFW_KEY_F1;
		static const Key F2				= GLFW_KEY_F2;
		static const Key F3				= GLFW_KEY_F3;
		static const Key F4				= GLFW_KEY_F4;
		static const Key F5				= GLFW_KEY_F5;
		static const Key F6				= GLFW_KEY_F6;
		static const Key F7				= GLFW_KEY_F7;
		static const Key F8				= GLFW_KEY_F8;
		static const Key F9				= GLFW_KEY_F9;
		static const Key F10			= GLFW_KEY_F10;
		static const Key F11			= GLFW_KEY_F11;
		static const Key F12			= GLFW_KEY_F12;
		static const Key F13			= GLFW_KEY_F13;
		static const Key F14			= GLFW_KEY_F14;
		static const Key F15			= GLFW_KEY_F15;
		static const Key F16			= GLFW_KEY_F16;
		static const Key F17			= GLFW_KEY_F17;
		static const Key F18			= GLFW_KEY_F18;
		static const Key F19			= GLFW_KEY_F19;
		static const Key F20			= GLFW_KEY_F20;
		static const Key F21			= GLFW_KEY_F21;
		static const Key F22			= GLFW_KEY_F22;
		static const Key F23			= GLFW_KEY_F23;
		static const Key F24			= GLFW_KEY_F24;
		static const Key F25			= GLFW_KEY_F25;
		static const Key KP_0			= GLFW_KEY_KP_0;
		static const Key KP_1			= GLFW_KEY_KP_1;
		static const Key KP_2			= GLFW_KEY_KP_2;
		static const Key KP_3			= GLFW_KEY_KP_3;
		static const Key KP_4			= GLFW_KEY_KP_4;
		static const Key KP_5			= GLFW_KEY_KP_5;
		static const Key KP_6			= GLFW_KEY_KP_6;
		static const Key KP_7			= GLFW_KEY_KP_7;
		static const Key KP_8			= GLFW_KEY_KP_8;
		static const Key KP_9			= GLFW_KEY_KP_9;
		static const Key KP_DECIMAL		= GLFW_KEY_KP_DECIMAL;
		static const Key KP_DIVIDE		= GLFW_KEY_KP_DIVIDE;
		static const Key KP_MULTIPLY	= GLFW_KEY_KP_MULTIPLY;
		static const Key KP_SUBTRACT	= GLFW_KEY_KP_SUBTRACT;
		static const Key KP_ADD			= GLFW_KEY_KP_ADD;
		static const Key KP_ENTER		= GLFW_KEY_KP_ENTER;
		static const Key KP_EQUAL		= GLFW_KEY_KP_EQUAL;
		static const Key LEFT_SHIFT		= GLFW_KEY_LEFT_SHIFT;
		static const Key LEFT_CONTROL	= GLFW_KEY_LEFT_CONTROL;
		static const Key LEFT_ALT		= GLFW_KEY_LEFT_ALT;
		static const Key LEFT_SUPER		= GLFW_KEY_LEFT_SUPER;
		static const Key RIGHT_SHIFT	= GLFW_KEY_RIGHT_SHIFT;
		static const Key RIGHT_CONTROL	= GLFW_KEY_RIGHT_CONTROL;
		static const Key RIGHT_ALT		= GLFW_KEY_RIGHT_ALT;
		static const Key RIGHT_SUPER	= GLFW_KEY_RIGHT_SUPER;
		static const Key MENU			= GLFW_KEY_MENU;
		static const Key NONE			= GLFW_KEY_MENU + 1;

		static const char* keyName[ NONE + 1 ];
		static const std::vector<Key> s_keysList;
	};
}