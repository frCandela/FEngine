#pragma once

#include "core/fanSingleton.h"

namespace fan {
//================================================================================================================================
//================================================================================================================================
	class Mouse : public Singleton<Mouse>
	{
		friend class Input;
	public:
		enum CursorState { disabled = GLFW_CURSOR_DISABLED, hidden = GLFW_CURSOR_HIDDEN, normal = GLFW_CURSOR_NORMAL };
		enum Button { button0 = 0, button1 = 1, button2, button3, button4, button5, button6, button7 };

		static btVector2	GetPosition()		{ return Get().m_position; }
		static btVector2	GetDelta()			{ return Get().m_delta; }
		static btVector2	GetDeltaScroll()	{ return Get().m_deltaScroll; }
		static btVector2	GetScreenSpacePosition();

		static void SetCursor(CursorState _state);
		static void LockCursor(bool _state, btVector2  _position = Get().m_position);

		static bool GetButtonDown(int  _GLFW_MOUSE_BUTTON);
		static bool GetButtonPressed(int _GLFW_MOUSE_BUTTON);
		static bool GetButtonReleased(int _GLFW_MOUSE_BUTTON);

	private:
		static void MouseCallback(GLFWwindow* window, double x, double y) { (void)window;	(void)x;	(void)y; }
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

		void Update();

		bool		m_lockCursor;
		btVector2	m_lockPosition;

		btVector2	m_oldPosition;
		btVector2	m_position;
		btVector2	m_delta;
		btVector2	m_deltaScroll;

		std::array< unsigned, 11 > m_buttonsPressed;
		std::array< unsigned, 11 > m_buttonsReleased;
	};
}