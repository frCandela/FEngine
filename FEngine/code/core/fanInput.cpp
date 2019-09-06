#include "fanGlobalIncludes.h"

#include "core/fanInput.h"

namespace fan {
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
}