#pragma once

namespace vk {

	class Window {
	public:
		Window(const char * _name, VkExtent2D size, VkInstance _vkInstance);
		~Window();

		const GLFWwindow *	GetWindow() const	{ return m_window; }
		GLFWwindow *		GetWindow()			{ return m_window; }
		VkSurfaceKHR		GetSurface()		{ return m_surface; }
		VkExtent2D			GetExtent();


	private:
		GLFWwindow * m_window;
		VkInstance m_vkInstance;
		VkSurfaceKHR m_surface;
	};
}