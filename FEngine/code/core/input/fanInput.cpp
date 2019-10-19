#include "fanGlobalIncludes.h"
#include "core/input/fanInput.h"

#include "core/input/fanKeyboard.h"
#include "core/input/fanMouse.h"
#include "core/input/fanInputManager.h"

namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	Input::Input() {
		m_eventManager = new InputManager();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Input::Setup(GLFWwindow * _window)
	{
		m_window = _window;

		int width, height;
		glfwGetWindowSize(_window, &width, &(height));
		m_windowSize = btVector2(static_cast<btScalar>(width), static_cast<btScalar>(height));

		double x, y;
		glfwGetCursorPos(_window, &x, &y);

		glfwSetFramebufferSizeCallback(_window, Input::WindowSizeCallback);
		glfwSetCursorPosCallback(_window, Mouse::MouseCallback);
		glfwSetMouseButtonCallback(_window, Mouse::MouseButtonCallback);
		glfwSetScrollCallback(_window, Mouse::ScrollCallback);
		glfwSetKeyCallback(_window, Keyboard::KeyCallback);
		glfwSetCharCallback(_window, Keyboard::CharCallback);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Input::WindowSizeCallback(GLFWwindow* _window, int _width, int _height) {
		Get().m_windowSize = btVector2(static_cast<btScalar>(_width), static_cast<btScalar>(_height));
		(void)_window;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Input::NewFrame()
	{
		++Get().m_count;
		glfwPollEvents();

		Mouse::Get().Update();
	}
}