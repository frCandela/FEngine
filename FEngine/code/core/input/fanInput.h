#pragma once

#include "core/fanSingleton.h"

namespace fan {
	class InputManager;

	//================================================================================================================================
	//================================================================================================================================
	class Input : public Singleton<Input>
	{
		friend class Singleton < Input>;

	public:
		void			Setup(GLFWwindow * _window);
		void			NewFrame();
		GLFWwindow *	Window()	 { return m_window; }
		btVector2		WindowSize() { return m_windowSize; }
		uint64_t		FrameCount() { return m_count; }
		InputManager&	Manager() { return *m_eventManager; }

	private:
		Input();

		GLFWwindow *	m_window;
		btVector2		m_windowSize;
		uint64_t		m_count;

		InputManager*	m_eventManager;

		 static void	WindowSizeCallback(GLFWwindow* window, int width, int height);
	};	
}
