#include "fanGlobalIncludes.h"

#include "core/input/fanMouse.h"

namespace fan {
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