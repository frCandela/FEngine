#pragma once

#include "core/fanSingleton.h"

namespace fan {
	class GameWindow;

//================================================================================================================================
//================================================================================================================================
	class Mouse : public Singleton<Mouse>
	{
		friend class Singleton<Mouse>;
		friend class Input;
	public:
		enum CursorState { disabled = GLFW_CURSOR_DISABLED, hidden = GLFW_CURSOR_HIDDEN, normal = GLFW_CURSOR_NORMAL };
		enum Button { button0 = 0, button1 = 1, button2, button3, button4, button5, button6, button7 };

		void Init( const GameWindow * _gameWindow ) { m_gameWindow = _gameWindow; }

		
		btVector2	GetDelta() { return m_delta; }
		btVector2	GetDeltaScroll() { return m_deltaScroll; }
		btVector2	GetScreenSpacePosition( const bool _localToGameWindow = true );
		btVector2	GetPosition( const bool _localToGameWindow = true );

		void SetCursor(CursorState _state);
		void LockCursor(bool _state, btVector2  _position = Get().m_position);

		bool GetButtonDown		( const int _GLFW_MOUSE_BUTTON, const bool _overrideUI = false );
		bool GetButtonPressed	( const int _GLFW_MOUSE_BUTTON, const bool _overrideUI = false );
		bool GetButtonReleased	( const int _GLFW_MOUSE_BUTTON, const bool _overrideUI = false );
	
	protected:
		Mouse();

	private:
		static void MouseCallback		(GLFWwindow* _window, double _x, double _y);
		static void MouseButtonCallback	(GLFWwindow* window, int button, int action, int mods);
		static void ScrollCallback		(GLFWwindow* window, double xoffset, double yoffset);

		void Update();

		const GameWindow * m_gameWindow = nullptr;

		bool		m_lockCursor;
		btVector2	m_lockPosition;

		btVector2	m_oldPosition;
		btVector2	m_position;
		btVector2	m_delta;
		btVector2	m_deltaScroll;

		std::array< uint64_t, 11 > m_buttonsPressed;
		std::array< uint64_t, 11 > m_buttonsReleased;
	};
}