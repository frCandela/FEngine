#pragma once

#include "core/fanSingleton.h"

namespace fan {


	//================================================================================================================================
	//================================================================================================================================
	class Input : public Singleton<Input>
	{
		friend class Singleton < Input>;

	public:
		void				Setup(GLFWwindow * _window);
		static void			NewFrame();
		static GLFWwindow * GetWindow()		{ return Get().m_window; }
		static btVector2	GetWindowSize() { return Get().m_windowSize; }
		static uint64_t		GetFrameCount() { return Get().m_count; }

	private:
		 GLFWwindow *	m_window;
		 btVector2		m_windowSize;
		 uint64_t		m_count;

		 static void		WindowSizeCallback(GLFWwindow* window, int width, int height);
	};	

	//================================================================================================================================
	//================================================================================================================================
	class KeyboardEvent {
	public:
		using Bitset = Bitset2::bitset2<GLFW_KEY_LAST>;
		KeyboardEvent( const char * _name, const Bitset _bisetKeys ) :
			m_bitsetKeys( _bisetKeys )
		{
			strncpy_s( m_name, 15, _name, 15 );
			m_name[15] = '\0';
		}
		const char * GetName() const { return m_name; }
		Bitset GetBitset() const { return m_bitsetKeys; }

	private:
		char m_name[16];
		Bitset m_bitsetKeys;

	};	

	//================================================================================================================================
	//================================================================================================================================
	namespace KeyMask {
		using Bitset = KeyboardEvent::Bitset;

		static const Bitset SPACE			= Bitset(1) << GLFW_KEY_SPACE;
		static const Bitset APOSTROPHE		= Bitset(1) << GLFW_KEY_APOSTROPHE;
		static const Bitset COMMA			= Bitset(1) << GLFW_KEY_COMMA;
		static const Bitset MINUS			= Bitset(1) << GLFW_KEY_MINUS;
		static const Bitset PERIOD			= Bitset(1) << GLFW_KEY_PERIOD;
		static const Bitset SLASH			= Bitset(1) << GLFW_KEY_SLASH;
		static const Bitset KEY0			= Bitset(1) << GLFW_KEY_0;
		static const Bitset KEY1			= Bitset(1) << GLFW_KEY_1;
		static const Bitset KEY2			= Bitset(1) << GLFW_KEY_2;
		static const Bitset KEY3			= Bitset(1) << GLFW_KEY_3;
		static const Bitset KEY4			= Bitset(1) << GLFW_KEY_4;
		static const Bitset KEY5			= Bitset(1) << GLFW_KEY_5;
		static const Bitset KEY6			= Bitset(1) << GLFW_KEY_6;
		static const Bitset KEY7			= Bitset(1) << GLFW_KEY_7;
		static const Bitset KEY8			= Bitset(1) << GLFW_KEY_8;
		static const Bitset KEY9			= Bitset(1) << GLFW_KEY_9;
		static const Bitset SEMICOLON		= Bitset(1) << GLFW_KEY_SEMICOLON;
		static const Bitset EQUAL			= Bitset(1) << GLFW_KEY_EQUAL;
		static const Bitset A				= Bitset(1) << GLFW_KEY_A;
		static const Bitset B				= Bitset(1) << GLFW_KEY_B;
		static const Bitset C				= Bitset(1) << GLFW_KEY_C;
		static const Bitset D				= Bitset(1) << GLFW_KEY_D;
		static const Bitset E				= Bitset(1) << GLFW_KEY_E;
		static const Bitset F				= Bitset(1) << GLFW_KEY_F;
		static const Bitset G				= Bitset(1) << GLFW_KEY_G;
		static const Bitset H				= Bitset(1) << GLFW_KEY_H;
		static const Bitset I				= Bitset(1) << GLFW_KEY_I;
		static const Bitset J				= Bitset(1) << GLFW_KEY_J;
		static const Bitset K				= Bitset(1) << GLFW_KEY_K;
		static const Bitset L				= Bitset(1) << GLFW_KEY_L;
		static const Bitset M				= Bitset(1) << GLFW_KEY_M;
		static const Bitset N				= Bitset(1) << GLFW_KEY_N;
		static const Bitset O				= Bitset(1) << GLFW_KEY_O;
		static const Bitset P				= Bitset(1) << GLFW_KEY_P;
		static const Bitset Q				= Bitset(1) << GLFW_KEY_Q;
		static const Bitset R				= Bitset(1) << GLFW_KEY_R;
		static const Bitset S				= Bitset(1) << GLFW_KEY_S;
		static const Bitset T				= Bitset(1) << GLFW_KEY_T;
		static const Bitset U				= Bitset(1) << GLFW_KEY_U;
		static const Bitset V				= Bitset(1) << GLFW_KEY_V;
		static const Bitset W				= Bitset(1) << GLFW_KEY_W;
		static const Bitset X				= Bitset(1) << GLFW_KEY_X;
		static const Bitset Y				= Bitset(1) << GLFW_KEY_Y;
		static const Bitset Z				= Bitset(1) << GLFW_KEY_Z;
		static const Bitset LEFT_BRACKET	= Bitset(1) << GLFW_KEY_LEFT_BRACKET;
		static const Bitset BACKSLASH		= Bitset(1) << GLFW_KEY_BACKSLASH;
		static const Bitset RIGHT_BRACKET	= Bitset(1) << GLFW_KEY_RIGHT_BRACKET;
		static const Bitset GRAVE_ACCENT	= Bitset(1) << GLFW_KEY_GRAVE_ACCENT;
		static const Bitset WORLD_1			= Bitset(1) << GLFW_KEY_WORLD_1;
		static const Bitset WORLD_2			= Bitset(1) << GLFW_KEY_WORLD_2;
		static const Bitset ESCAPE			= Bitset(1) << GLFW_KEY_ESCAPE;
		static const Bitset ENTER			= Bitset(1) << GLFW_KEY_ENTER;
		static const Bitset TAB				= Bitset(1) << GLFW_KEY_TAB;
		static const Bitset BACKSPACE		= Bitset(1) << GLFW_KEY_BACKSPACE;
		static const Bitset INSERT			= Bitset(1) << GLFW_KEY_INSERT;
		static const Bitset DELETE			= Bitset(1) << GLFW_KEY_DELETE;
		static const Bitset RIGHT			= Bitset(1) << GLFW_KEY_RIGHT;
		static const Bitset LEFT			= Bitset(1) << GLFW_KEY_LEFT;
		static const Bitset DOWN			= Bitset(1) << GLFW_KEY_DOWN;
		static const Bitset UP				= Bitset(1) << GLFW_KEY_UP;
		static const Bitset PAGE_UP			= Bitset(1) << GLFW_KEY_PAGE_UP;
		static const Bitset PAGE_DOWN		= Bitset(1) << GLFW_KEY_PAGE_DOWN;
		static const Bitset HOME			= Bitset(1) << GLFW_KEY_HOME;
		static const Bitset END				= Bitset(1) << GLFW_KEY_END;
		static const Bitset CAPS_LOCK		= Bitset(1) << GLFW_KEY_CAPS_LOCK;
		static const Bitset SCROLL_LOCK		= Bitset(1) << GLFW_KEY_SCROLL_LOCK;
		static const Bitset NUM_LOCK		= Bitset(1) << GLFW_KEY_NUM_LOCK;
		static const Bitset PRINT_SCREEN	= Bitset(1) << GLFW_KEY_PRINT_SCREEN;
		static const Bitset PAUSE			= Bitset(1) << GLFW_KEY_PAUSE;
		static const Bitset F1				= Bitset(1) << GLFW_KEY_F1;
		static const Bitset F2				= Bitset(1) << GLFW_KEY_F2;
		static const Bitset F3				= Bitset(1) << GLFW_KEY_F3;
		static const Bitset F4				= Bitset(1) << GLFW_KEY_F4;
		static const Bitset F5				= Bitset(1) << GLFW_KEY_F5;
		static const Bitset F6				= Bitset(1) << GLFW_KEY_F6;
		static const Bitset F7				= Bitset(1) << GLFW_KEY_F7;
		static const Bitset F8				= Bitset(1) << GLFW_KEY_F8;
		static const Bitset F9				= Bitset(1) << GLFW_KEY_F9;
		static const Bitset F10				= Bitset(1) << GLFW_KEY_F10;
		static const Bitset F11				= Bitset(1) << GLFW_KEY_F11;
		static const Bitset F12				= Bitset(1) << GLFW_KEY_F12;
		static const Bitset F13				= Bitset(1) << GLFW_KEY_F13;
		static const Bitset F14				= Bitset(1) << GLFW_KEY_F14;
		static const Bitset F15				= Bitset(1) << GLFW_KEY_F15;
		static const Bitset F16				= Bitset(1) << GLFW_KEY_F16;
		static const Bitset F17				= Bitset(1) << GLFW_KEY_F17;
		static const Bitset F18				= Bitset(1) << GLFW_KEY_F18;
		static const Bitset F19				= Bitset(1) << GLFW_KEY_F19;
		static const Bitset F20				= Bitset(1) << GLFW_KEY_F20;
		static const Bitset F21				= Bitset(1) << GLFW_KEY_F21;
		static const Bitset F22				= Bitset(1) << GLFW_KEY_F22;
		static const Bitset F23				= Bitset(1) << GLFW_KEY_F23;
		static const Bitset F24				= Bitset(1) << GLFW_KEY_F24;
		static const Bitset F25				= Bitset(1) << GLFW_KEY_F25;
		static const Bitset KP_0			= Bitset(1) << GLFW_KEY_KP_0;
		static const Bitset KP_1			= Bitset(1) << GLFW_KEY_KP_1;
		static const Bitset KP_2			= Bitset(1) << GLFW_KEY_KP_2;
		static const Bitset KP_3			= Bitset(1) << GLFW_KEY_KP_3;
		static const Bitset KP_4			= Bitset(1) << GLFW_KEY_KP_4;
		static const Bitset KP_5			= Bitset(1) << GLFW_KEY_KP_5;
		static const Bitset KP_6			= Bitset(1) << GLFW_KEY_KP_6;
		static const Bitset KP_7			= Bitset(1) << GLFW_KEY_KP_7;
		static const Bitset KP_8			= Bitset(1) << GLFW_KEY_KP_8;
		static const Bitset KP_9			= Bitset(1) << GLFW_KEY_KP_9;
		static const Bitset KP_DECIMAL		= Bitset(1) << GLFW_KEY_KP_DECIMAL;
		static const Bitset KP_DIVIDE		= Bitset(1) << GLFW_KEY_KP_DIVIDE;
		static const Bitset KP_MULTIPLY		= Bitset(1) << GLFW_KEY_KP_MULTIPLY;
		static const Bitset KP_SUBTRACT		= Bitset(1) << GLFW_KEY_KP_SUBTRACT;
		static const Bitset KP_ADD			= Bitset(1) << GLFW_KEY_KP_ADD;
		static const Bitset KP_ENTER		= Bitset(1) << GLFW_KEY_KP_ENTER;
		static const Bitset KP_EQUAL		= Bitset(1) << GLFW_KEY_KP_EQUAL;
		static const Bitset LEFT_SHIFT		= Bitset(1) << GLFW_KEY_LEFT_SHIFT	;
		static const Bitset LEFT_CONTROL	= Bitset(1) << GLFW_KEY_LEFT_CONTROL;
		static const Bitset LEFT_ALT		= Bitset(1) << GLFW_KEY_LEFT_ALT;
		static const Bitset LEFT_SUPER		= Bitset(1) << GLFW_KEY_LEFT_SUPER	;
		static const Bitset RIGHT_SHIFT		= Bitset(1) << GLFW_KEY_RIGHT_SHIFT;
		static const Bitset RIGHT_CONTROL	= Bitset(1) << GLFW_KEY_RIGHT_CONTROL;
		static const Bitset RIGHT_ALT		= Bitset(1) << GLFW_KEY_RIGHT_ALT;
		static const Bitset RIGHT_SUPER		= Bitset(1) << GLFW_KEY_RIGHT_SUPER;
		static const Bitset MENU			= Bitset(1) << GLFW_KEY_MENU;
	}
}