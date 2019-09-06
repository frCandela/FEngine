#pragma once

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class Input
	{
	public:
		friend class Renderer;

		static void			Setup(GLFWwindow * _window);
		static void			NewFrame();
		static GLFWwindow * GetWindow() { return m_window; }
		static btVector2	GetWindowSize() { return m_windowSize; }
		static unsigned		GetFrameCount() { return m_count; }

	private:
		static void			WindowSizeCallback(GLFWwindow* window, int width, int height);
		static GLFWwindow * m_window;
		static btVector2	m_windowSize;
		static unsigned		m_count;
	};			
}