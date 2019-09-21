#pragma once

#include "core/fanSingleton.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class Input : public Singleton<Input>
	{
		friend class Singleton < Input>;

	public:
		void				Setup(GLFWwindow * _window);
		static void			NewFrame();
		static GLFWwindow * GetWindow()		{ return Get().m_window; }
		static btVector2	GetWindowSize() { return Get().m_windowSize; }
		static uint64_t		GetFrameCount() { return Get().m_count; }

	private:
		 GLFWwindow *	m_window;
		 btVector2		m_windowSize;
		 uint64_t		m_count;

		 static void		WindowSizeCallback(GLFWwindow* window, int width, int height);
	};			
}