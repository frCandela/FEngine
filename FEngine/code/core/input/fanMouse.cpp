#include "fanGlobalIncludes.h"

#include "core/input/fanMouse.h"
#include "core/input/fanInput.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	Mouse::Mouse() {
		assert( m_buttonsPressed.size() == m_buttonsReleased.size() );
		const uint64_t max = std::numeric_limits<uint64_t>::max();
		for (int buttonIndex = 0; buttonIndex < m_buttonsPressed.size(); buttonIndex++) {
			m_buttonsPressed[buttonIndex]  = max;
			m_buttonsReleased[buttonIndex] = max;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mouse::SetCursor(CursorState _state)				{ 
		glfwSetInputMode(Input::Get().Window(), GLFW_CURSOR, _state);
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Mouse::GetButtonDown(const int  _GLFW_MOUSE_BUTTON, const bool _overrideUI ) {
		if ( !_overrideUI && ImGui::GetIO().WantCaptureMouse) {
			return false;
		} else {
			return glfwGetMouseButton(Input::Get().Window(), _GLFW_MOUSE_BUTTON) == GLFW_PRESS;
		} 
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Mouse::GetButtonPressed(const int _GLFW_MOUSE_BUTTON, const bool _overrideUI )	{
		if ( ! _overrideUI && ImGui::GetIO().WantCaptureMouse) {
			return false;
		}
		else {
			return Get().m_buttonsPressed[_GLFW_MOUSE_BUTTON] == Input::Get().FrameCount();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Mouse::GetButtonReleased(const int _GLFW_MOUSE_BUTTON, const bool _overrideUI )	{
		if ( !_overrideUI && ImGui::GetIO().WantCaptureMouse) {
			return false;
		}
		else {
			return Get().m_buttonsReleased[_GLFW_MOUSE_BUTTON] == Input::Get().FrameCount();
		}
	}

	//================================================================================================================================
	// Coordinate between -1.f and 1.f
	//================================================================================================================================
	btVector2 Mouse::GetScreenSpacePosition()
	{
		btVector2 screenSize = Input::Get().WindowSizeF();
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
			glfwSetCursorPos(Input::Get().Window(), Get().m_lockPosition.x(), Get().m_lockPosition.y());
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mouse::ScrollCallback(GLFWwindow* /*_window*/, double _xoffset, double _yoffset)
	{		
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += (float)_xoffset;
		io.MouseWheel += (float)_yoffset;

		Get().m_deltaScroll += btVector2(static_cast<float>(_xoffset), static_cast<float>(_yoffset));
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
		if (_button < 5) {
			ImGuiIO& io = ImGui::GetIO();
			if (_action == GLFW_PRESS) {
				io.MouseDown[_button] = true;
			}
			else if (_action == GLFW_RELEASE) {
				io.MouseDown[_button] = false;
			}
		}

		if (_action == GLFW_PRESS) {
			Get().m_buttonsPressed[_button] = Input::Get().FrameCount();
		}
		else if (_action == GLFW_RELEASE) {
			Get().m_buttonsReleased[_button] = Input::Get().FrameCount();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mouse::Update()
	{
		//Mouse
		double x, y;
		glfwGetCursorPos(Input::Get().Window(), &x, &y);

		if (m_lockCursor)
		{
			GLFWwindow * window = Input::Get().Window();
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