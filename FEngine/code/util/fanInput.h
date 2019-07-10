#pragma once

class Input
{
public:
	static void			Setup(GLFWwindow * _window);
	static void			NewFrame();
	static GLFWwindow * GetWindow() { return m_window; }
	static glm::ivec2	GetWindowSize() { return m_windowSize; }
	static unsigned		GetFrameCount() { return m_count; }

private:
	static void			WindowSizeCallback(GLFWwindow* window, int width, int height);
	static GLFWwindow * m_window;
	static glm::ivec2	m_windowSize;
	static unsigned		m_count;
};

class Keyboard
{
	friend class Input;

public:
	static int	IsKeyDown(int _GLFW_KEY)		{ return glfwGetKey(Input::GetWindow(), _GLFW_KEY) == GLFW_PRESS; }
	static bool IsKeyPressed(int _GLFW_KEY)		{ return m_keysPressed[_GLFW_KEY] == Input::GetFrameCount(); }
	static bool IsKeyReleased(int _GLFW_KEY)	{ return m_keysReleased[_GLFW_KEY] == Input::GetFrameCount(); }

	enum AzertyKey {
		D = GLFW_KEY_D,
		Q = GLFW_KEY_A,
		S = GLFW_KEY_S,
		Z = GLFW_KEY_W,
		A = GLFW_KEY_Q,
		E = GLFW_KEY_E
	};

private:
	static void KeyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods);
	static void CharCallback(GLFWwindow* _window, unsigned int _c);

	//Events
	static std::array< unsigned, 349 > m_keysPressed;
	static std::array< unsigned, 349 > m_keysReleased;
};

class Mouse
{
	friend class Input;

public:
	enum CursorState { disabled = GLFW_CURSOR_DISABLED, hidden = GLFW_CURSOR_HIDDEN, normal = GLFW_CURSOR_NORMAL };
	enum Button { button0 = 0, button1 = 1, button2, button3, button4, button5, button6, button7 };

	static btVector2	GetPosition()			{ return m_position; }
	static btVector2	GetDelta()				{ return m_delta; }
	static btVector2	GetDeltaScroll()		{ return m_deltaScroll; }
	static btVector2	GetScreenSpacePosition(btVector2 screenSize);

	static void SetCursor(CursorState _state) { glfwSetInputMode(Input::GetWindow(), GLFW_CURSOR, _state); }
	static void LockCursor(bool _state, btVector2  _position = m_position);

	static bool IsKeyDown(int  _GLFW_MOUSE_BUTTON)			{ return glfwGetMouseButton(Input::GetWindow(), _GLFW_MOUSE_BUTTON) == GLFW_PRESS; }
	static bool IsButtonPressed(int _GLFW_MOUSE_BUTTON)		{ return m_buttonsPressed[_GLFW_MOUSE_BUTTON] == Input::GetFrameCount(); }
	static bool IsButtonReleased(int _GLFW_MOUSE_BUTTON)	{ return m_buttonsReleased[_GLFW_MOUSE_BUTTON] == Input::GetFrameCount(); }

private:
	static void MouseCallback		(GLFWwindow* window, double x, double y) { (void)window;	(void)x;	(void)y; }
	static void MouseButtonCallback	(GLFWwindow* window, int button, int action, int mods);
	static void ScrollCallback		(GLFWwindow* window, double xoffset, double yoffset);

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