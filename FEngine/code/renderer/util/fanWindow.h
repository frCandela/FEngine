#pragma once

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class Window {
	public:
		Window(const char * _name, const VkExtent2D size, const glm::ivec2 _position, VkInstance _vkInstance);
		~Window();

		const GLFWwindow *	GetWindow() const { return m_window; }
		GLFWwindow *		GetWindow() { return m_window; }
		VkSurfaceKHR		GetSurface() { return m_surface; }
		VkExtent2D			GetExtent() const;
		glm::ivec2			GetPosition() const;

	private:
		GLFWwindow * m_window;
		VkInstance m_vkInstance;
		VkSurfaceKHR m_surface;
	};
}