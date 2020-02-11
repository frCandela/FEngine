#include "core/input/fanInput.hpp"
#include "core/input/fanKeyboard.hpp"
#include "core/input/fanMouse.hpp"
#include "core/input/fanInputManager.hpp"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	Input::Input()
	{
		m_eventManager = new InputManager();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Input::Setup( GLFWwindow* _window )
	{
		m_window = _window;

		int width, height;
		glfwGetWindowSize( _window, &width, &( height ) );
		m_windowSize = glm::ivec2( width, height );

		double x, y;
		glfwGetCursorPos( _window, &x, &y );

		glfwSetFramebufferSizeCallback( _window, Input::WindowSizeCallback );
		glfwSetCursorPosCallback( _window, Mouse::MouseCallback );
		glfwSetMouseButtonCallback( _window, Mouse::MouseButtonCallback );
		glfwSetScrollCallback( _window, Mouse::ScrollCallback );
		glfwSetKeyCallback( _window, Keyboard::KeyCallback );
		glfwSetCharCallback( _window, Keyboard::CharCallback );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Input::WindowSizeCallback( GLFWwindow* _window, int _width, int _height )
	{
		Get().m_windowSize = glm::ivec2( _width, _height );
		( void ) _window;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Input::NewFrame()
	{
		++Get().m_count;
		glfwPollEvents();
	}
}