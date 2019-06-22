#pragma once

class Input
{
public:
	static void Setup(GLFWwindow * window);
	static GLFWwindow * GetWindow();
	static glm::ivec2 GetWindowSize();
	static unsigned FrameCount();
	static bool ShuttingDown();

	static void NewFrame();
private:
	static void window_size_callback(GLFWwindow* window, int width, int height);
	static GLFWwindow * m_window;
	static glm::ivec2 m_windowSize;
	static unsigned m_count;
};

class Keyboard
{
	friend class Input;

public:
	static int KeyDown(int GLFW_KEY);
	static bool KeyPressed(int GLFW_KEY);
	static bool KeyReleased(int GLFW_KEY);

	enum AzertyKey {
		D = GLFW_KEY_D,
		Q = GLFW_KEY_A,
		S = GLFW_KEY_S,
		Z = GLFW_KEY_W,
		A = GLFW_KEY_Q,
		E = GLFW_KEY_E
	};

private:
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void char_callback(GLFWwindow*, unsigned int c);

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

	static glm::vec2 Position();
	static glm::vec2 ScreenSpacePosition(glm::vec2 screenSize);
	static glm::vec2 Delta();
	static glm::ivec2 DeltaScroll();

	static void SetCursor(CursorState state);
	static void LockCursor(bool state, glm::vec2  position = m_position);

	static bool KeyDown(int GLFW_MOUSE_BUTTON);
	static bool ButtonPressed(int GLFW_MOUSE_BUTTON);
	static bool ButtonReleased(int GLFW_MOUSE_BUTTON);

private:
	static void mouse_callback(GLFWwindow* window, double x, double y);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static void Update();

	static bool m_lockCursor;
	static glm::vec2 m_lockPosition;

	static glm::vec2 m_oldPosition;
	static glm::vec2 m_position;
	static glm::vec2 m_delta;
	static glm::ivec2 m_deltaScroll;

	static std::array< unsigned, 11 > m_buttonsPressed;
	static std::array< unsigned, 11 > m_buttonsReleased;
};