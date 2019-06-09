#pragma once

#include "AllInclude.h"

class Window {
public:
	Window( const char * _name, VkExtent2D size, VkInstance _instance ) {
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		m_window = glfwCreateWindow(size.width, size.height, _name, nullptr/* fullscreen monitor */, nullptr);
		glfwCreateWindowSurface(_instance, m_window, nullptr, &m_surface);
	}
	~Window() {
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}
	

	const GLFWwindow * GetWindow() const { return m_window; }
	GLFWwindow * GetWindow() { return m_window; }
	void GetFramebufferSize( int & width, int & height ){ glfwGetFramebufferSize(m_window, &width, &height); }
	VkSurfaceKHR GetSurface() { return m_surface;  }

private:
	GLFWwindow * m_window;
	VkSurfaceKHR m_surface;

};