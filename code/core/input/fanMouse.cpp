#include "core/input/fanMouse.hpp"

#include "imgui/imgui.h"
#include "core/input/fanInput.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Mouse::Mouse()
	{
		assert( m_buttonsPressed.size() == m_buttonsReleased.size() );
		constexpr uint64_t max = std::numeric_limits<uint64_t>::max();
		for ( int buttonIndex = 0; buttonIndex < m_buttonsPressed.size(); buttonIndex++ )
		{
			m_buttonsPressed[ buttonIndex ] = max;
			m_buttonsReleased[ buttonIndex ] = max;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mouse::SetCursor( const CursorState _state )
	{
		glfwSetInputMode( Input::Get().Window(), GLFW_CURSOR, _state );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Mouse::GetButtonDown( const int  _GLFW_MOUSE_BUTTON, const bool _overrideUI )
	{
		if ( _overrideUI || m_windowHovered )
		{
			return glfwGetMouseButton( Input::Get().Window(), _GLFW_MOUSE_BUTTON ) == GLFW_PRESS;
		}
		else
		{
			return false;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Mouse::GetButtonPressed( const int _GLFW_MOUSE_BUTTON, const bool _overrideUI )
	{
		if ( _overrideUI || m_windowHovered )
		{
			return m_buttonsPressed[ _GLFW_MOUSE_BUTTON ] == Input::Get().FrameCount();
		}
		else
		{
			return false;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Mouse::GetButtonReleased( const int _GLFW_MOUSE_BUTTON, const bool _overrideUI )
	{
		if ( _overrideUI || m_windowHovered )
		{
			return m_buttonsReleased[ _GLFW_MOUSE_BUTTON ] == Input::Get().FrameCount();
		}
		else
		{
			return false;
		}
	}

	//================================================================================================================================
	// Coordinate between -1.f and 1.f
	//================================================================================================================================
	btVector2 Mouse::GetScreenSpacePosition( const bool _localToGameWindow )
	{
		const btVector2 offset = _localToGameWindow ? m_windowOffset : btVector2::Zero();
		const btVector2 size = _localToGameWindow ? m_windowSize : Input::Get().WindowSizeF();

		btVector2 ratio = 2.f * ( m_position - offset ) / size - btVector2( 1.f, 1.f );
		ratio.setX( std::clamp( ratio.x(), -1.f, 1.f ) );
		ratio.setY( std::clamp( ratio.y(), -1.f, 1.f ) );
		return ratio;
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector2	Mouse::GetPosition( const bool _localToGameWindow )
	{
		const btVector2 offset = _localToGameWindow ? m_windowOffset : btVector2::Zero();
		return m_position - offset;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mouse::LockCursor( const bool _state, const btVector2& _position )
	{
		if ( m_lockCursor != _state )
		{
			m_lockPosition = _position;
			m_lockCursor = _state;
			glfwSetCursorPos( Input::Get().Window(), m_lockPosition.x(), m_lockPosition.y() );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mouse::ScrollCallback( GLFWwindow* /*_window*/, double _xoffset, double _yoffset )
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += ( float ) _xoffset;
		io.MouseWheel += ( float ) _yoffset;

		Get().m_deltaScroll += btVector2( static_cast< float >( _xoffset ), static_cast< float >( _yoffset ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mouse::MouseCallback( GLFWwindow* /*window*/, double _x, double _y )
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2( static_cast< float >( _x ), static_cast< float >( _y ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mouse::MouseButtonCallback( GLFWwindow* /*_window*/, int _button, int _action, int /*_mods*/ )
	{
		if ( _button < 5 )
		{
			ImGuiIO& io = ImGui::GetIO();
			if ( _action == GLFW_PRESS )
			{
				io.MouseDown[ _button ] = true;
			}
			else if ( _action == GLFW_RELEASE )
			{
				io.MouseDown[ _button ] = false;
			}
		}

		if ( _action == GLFW_PRESS )
		{
			Get().m_buttonsPressed[ _button ] = Input::Get().FrameCount();
		}
		else if ( _action == GLFW_RELEASE )
		{
			Get().m_buttonsReleased[ _button ] = Input::Get().FrameCount();
		}
	}

	//================================================================================================================================
	// Called every frame to update buffered values
	//================================================================================================================================
	void Mouse::Update( const btVector2& _windowOffset, const btVector2& _windowSize, const bool _windowHovered )
	{
		//Mouse
		double x, y;
		glfwGetCursorPos( Input::Get().Window(), &x, &y );

		if ( m_lockCursor )
		{
			GLFWwindow* window = Input::Get().Window();
			glfwSetCursorPos( window, m_lockPosition.x(), m_lockPosition.y() );

			m_position = btVector2( static_cast< btScalar >( x ), static_cast< btScalar >( y ) );
			m_delta = m_position - m_lockPosition;
		}
		else
		{
			m_oldPosition = m_position;
			m_position = btVector2( static_cast< btScalar >( x ), static_cast< btScalar >( y ) );
			m_delta = m_position - m_oldPosition;
		}

		m_deltaScroll = btVector2();
		m_windowOffset = _windowOffset;
		m_windowSize = _windowSize;
		m_windowHovered = _windowHovered;
	}
}