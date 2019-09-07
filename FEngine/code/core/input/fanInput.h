#pragma once

#include "core/fanSingleton.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class Input : public Singleton<Input>
	{
	public:
		friend class Renderer;

		void				Setup(GLFWwindow * _window);
		static void			NewFrame();
		static GLFWwindow * GetWindow()		{ return Get().m_window; }
		static btVector2	GetWindowSize() { return Get().m_windowSize; }
		static unsigned		GetFrameCount() { return Get().m_count; }

	private:
		 GLFWwindow * m_window;
		 btVector2	m_windowSize;
		 unsigned		m_count;

		 static void			WindowSizeCallback(GLFWwindow* window, int width, int height);
	};			
}