#include "fanGlobalIncludes.h"

#include "core/input/fanMouse.h"
#include "core/input/fanInput.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	void Mouse::SetCursor(CursorState _state)				{ 
		glfwSetInputMode(Input::GetWindow(), GLFW_CURSOR, _state); 
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Mouse::GetButtonDown(int  _GLFW_MOUSE_BUTTON)		{ 
		if (ImGui::GetIO().WantCaptureMouse == false || true) 
		{
			return glfwGetMouseButton(Input::GetWindow(), _GLFW_MOUSE_BUTTON) == GLFW_PRESS;
		} else {
			return false;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Mouse::GetButtonPressed(int _GLFW_MOUSE_BUTTON)	{ 
		return Get().m_buttonsPressed[_GLFW_MOUSE_BUTTON] == Input::GetFrameCount(); 
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Mouse::GetButtonReleased(int _GLFW_MOUSE_BUTTON)	{ 
		return Get().m_buttonsReleased[_GLFW_MOUSE_BUTTON] == Input::GetFrameCount(); 
	}

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
		if (Get().m_lockCursor != _state)
		{
			Get().m_lockPosition = _position;
			Get().m_lockCursor = _state;
			glfwSetCursorPos(Input::GetWindow(), Get().m_lockPosition.x(), Get().m_lockPosition.y());
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mouse::ScrollCallback(GLFWwindow* /*_window*/, double _xoffset, double _yoffset)
	{		
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += (float)_xoffset;
		io.MouseWheel += (float)_yoffset;

		if (ImGui::GetIO().WantCaptureMouse == false) {
			Get().m_deltaScroll += btVector2(static_cast<float>(_xoffset), static_cast<float>(_yoffset));
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mouse::MouseCallback(GLFWwindow* /*window*/, double _x, double _y) {
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2( static_cast<float>(_x), static_cast<float>(_y));
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mouse::MouseButtonCallback(GLFWwindow* /*_window*/, int _button, int _action, int /*_mods*/)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[0] = Mouse::GetButtonDown(Mouse::button0);
		io.MouseDown[1] = Mouse::GetButtonDown(Mouse::button1);

		if ( io.WantCaptureMouse == false ) {
			if (_action == GLFW_PRESS) {
				Get().m_buttonsPressed[_button] = Input::GetFrameCount();
			}
			else if (_action == GLFW_RELEASE) {
				Get().m_buttonsReleased[_button] = Input::GetFrameCount();
			}
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