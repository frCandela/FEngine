#pragma once

namespace fan {
//================================================================================================================================
//================================================================================================================================
	class Mouse
	{
		friend class Input;
	public:
		enum CursorState { disabled = GLFW_CURSOR_DISABLED, hidden = GLFW_CURSOR_HIDDEN, normal = GLFW_CURSOR_NORMAL };
		enum Button { button0 = 0, button1 = 1, button2, button3, button4, button5, button6, button7 };

		static btVector2	GetPosition() { return m_position; }
		static btVector2	GetDelta() { return m_delta; }
		static btVector2	GetDeltaScroll() { return m_deltaScroll; }
		static btVector2	GetScreenSpacePosition();

		static void SetCursor(CursorState _state) { glfwSetInputMode(Input::GetWindow(), GLFW_CURSOR, _state); }
		static void LockCursor(bool _state, btVector2  _position = m_position);

		static bool GetButtonDown(int  _GLFW_MOUSE_BUTTON) { return glfwGetMouseButton(Input::GetWindow(), _GLFW_MOUSE_BUTTON) == GLFW_PRESS; }
		static bool GetButtonPressed(int _GLFW_MOUSE_BUTTON) { return m_buttonsPressed[_GLFW_MOUSE_BUTTON] == Input::GetFrameCount(); }
		static bool GetButtonReleased(int _GLFW_MOUSE_BUTTON) { return m_buttonsReleased[_GLFW_MOUSE_BUTTON] == Input::GetFrameCount(); }

	private:
		static void MouseCallback(GLFWwindow* window, double x, double y) { (void)window;	(void)x;	(void)y; }
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

		static void Update();

		static bool			m_lockCursor;
		static btVector2	m_lockPosition;

		static btVector2	m_oldPosition;
		static btVector2	m_position;
		static btVector2	m_delta;
		static btVector2	m_deltaScroll;

		static std::array< unsigned, 11 > m_buttonsPressed;
		static std::array< unsigned, 11 > m_buttonsReleased;
	};
}