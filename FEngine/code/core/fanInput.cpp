#include "fanGlobalIncludes.h"

#include "core/fanInput.h"

namespace fan {
	//================================================================================================================================
	// INPUT
	//================================================================================================================================
	unsigned Input::m_count = 0;
	GLFWwindow * Input::m_window = nullptr;
	btVector2 Input::m_windowSize;

	//================================================================================================================================
	//================================================================================================================================
	void Input::Setup(GLFWwindow * _window)
	{
		m_window = _window;

		int width, height;
		glfwGetWindowSize(_window, &width, &(height));
		m_windowSize = btVector2(static_cast<btScalar>(width), static_cast<btScalar>(height));

		double x, y;
		glfwGetCursorPos(m_window, &x, &y);

		glfwSetFramebufferSizeCallback(m_window, Input::WindowSizeCallback);
		glfwSetCursorPosCallback(m_window, Mouse::MouseCallback);

		glfwSetMouseButtonCallback(m_window, Mouse::MouseButtonCallback);
		glfwSetScrollCallback(_window, Mouse::ScrollCallback);
		glfwSetKeyCallback(m_window, Keyboard::KeyCallback);
		glfwSetCharCallback(_window, Keyboard::CharCallback);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Input::WindowSizeCallback(GLFWwindow* _window, int _width, int _height) {
		m_windowSize = btVector2(static_cast<btScalar>(_width), static_cast<btScalar>(_height));
		(void)_window;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Input::NewFrame()
	{
		++m_count;
		glfwPollEvents();

		Mouse::Update();

		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(Mouse::GetPosition().x(), Mouse::GetPosition().y());
		io.MouseDown[0] = Mouse::GetButtonDown(Mouse::button0);
		io.MouseDown[1] = Mouse::GetButtonDown(Mouse::button1);
	}

	//================================================================================================================================
	// KEYBOARD
	//================================================================================================================================
	std::array< unsigned, 349 > Keyboard::m_keysPressed;
	std::array< unsigned, 349 > Keyboard::m_keysReleased;

	//================================================================================================================================
	//================================================================================================================================
	void Keyboard::KeyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
	{
		(void)_scancode;
		(void)_window;

		//Imgui
		ImGuiIO& io = ImGui::GetIO();
		if (_action == GLFW_PRESS)
			io.KeysDown[_key] = true;
		if (_action == GLFW_RELEASE)
			io.KeysDown[_key] = false;

		(void)_mods; // Modifiers are not reliable across systems
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

		//Keyboard
		if (_action == GLFW_PRESS)
			m_keysPressed[_key] = Input::GetFrameCount();
		else if (_action == GLFW_RELEASE)
			m_keysReleased[_key] = Input::GetFrameCount();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Keyboard::CharCallback(GLFWwindow* _window, unsigned int _c)
	{
		(void)_window;
		ImGuiIO& io = ImGui::GetIO();
		if (_c > 0 && _c < 0x10000)
			io.AddInputCharacter((unsigned short)_c);
	}


	//================================================================================================================================
	// MOUSE
	//================================================================================================================================
	std::array< unsigned, 11 > Mouse::m_buttonsPressed = { (unsigned)-1,(unsigned)-1,(unsigned)-1 ,(unsigned)-1 ,(unsigned)-1 ,(unsigned)-1 ,(unsigned)-1 ,(unsigned)-1 ,(unsigned)-1 ,(unsigned)-1 ,(unsigned)-1 };
	std::array< unsigned, 11 > Mouse::m_buttonsReleased;
	btVector2 Mouse::m_lockPosition;
	bool Mouse::m_lockCursor = false;
	btVector2 Mouse::m_oldPosition;
	btVector2 Mouse::m_position;
	btVector2 Mouse::m_delta;
	btVector2 Mouse::m_deltaScroll;

	//================================================================================================================================
	// Coordinate between -1.f and 1.f
	//================================================================================================================================
	btVector2 Mouse::GetScreenSpacePosition()
	{
		btVector2 screenSize = Input::GetWindowSize();
		btVector2 ratio = 2.f * Mouse::GetPosition() / screenSize - btVector2(1.f, 1.f);
		ratio.setX(std::clamp(ratio.x(), -1.f, 1.f));
		ratio.setY(std::clamp(ratio.y(), -1.f, 1.f));
		return ratio;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mouse::LockCursor(bool _state, btVector2 _position)
	{
		if (m_lockCursor != _state)
		{
			m_lockPosition = _position;
			m_lockCursor = _state;
			glfwSetCursorPos(Input::GetWindow(), m_lockPosition.x(), m_lockPosition.y());
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mouse::ScrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset)
	{
		(void)_window;

		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += (float)_xoffset;
		io.MouseWheel += (float)_yoffset;

		m_deltaScroll += btVector2(static_cast<float>(_xoffset), static_cast<float>(_yoffset));
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mouse::MouseButtonCallback(GLFWwindow* _window, int _button, int _action, int _mods)
	{
		(void)_mods;
		(void)_window;

		if (_action == GLFW_PRESS)
		{
			m_buttonsPressed[_button] = Input::GetFrameCount();
		}
		else if (_action == GLFW_RELEASE)
		{
			m_buttonsReleased[_button] = Input::GetFrameCount();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mouse::Update()
	{
		//Mouse
		double x, y;
		glfwGetCursorPos(Input::GetWindow(), &x, &y);

		if (m_lockCursor)
		{
			GLFWwindow * window = Input::GetWindow();
			glfwSetCursorPos(window, m_lockPosition.x(), m_lockPosition.y());

			m_position = btVector2(static_cast<btScalar>(x), static_cast<btScalar>(y));
			m_delta = m_position - m_lockPosition;
		}
		else
		{
			m_oldPosition = m_position;
			m_position = btVector2(static_cast<btScalar>(x), static_cast<btScalar>(y));
			m_delta = m_position - m_oldPosition;
		}

		m_deltaScroll = btVector2();
	}
}