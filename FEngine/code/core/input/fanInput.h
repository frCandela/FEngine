#pragma once

#include "core/fanSingleton.h"

namespace fan {
	class EventManager;

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
		EventManager&	Events() { return *m_eventManager; }

	private:
		Input();

		GLFWwindow *	m_window;
		btVector2		m_windowSize;
		uint64_t		m_count;

		EventManager*	m_eventManager;

		 static void	WindowSizeCallback(GLFWwindow* window, int width, int height);
	};	
}
