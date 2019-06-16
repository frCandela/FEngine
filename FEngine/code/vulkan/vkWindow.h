#pragma once

#include "AllInclude.h"

namespace vk {
	class Window {
	public:
		Window(const char * _name, VkExtent2D size, VkInstance _vkInstance) :
			m_vkInstance(_vkInstance) {
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
			m_window = glfwCreateWindow(size.width, size.height, _name, nullptr/* fullscreen monitor */, nullptr);
			glfwCreateWindowSurface(_vkInstance, m_window, nullptr, &m_surface);
			std::cout << std::hex << "VkSurfaceKHR\t\t" << m_surface << std::dec << std::endl;
		}
		~Window() {
			vkDestroySurfaceKHR(m_vkInstance, m_surface, nullptr);
			m_surface = VK_NULL_HANDLE;
			glfwDestroyWindow(m_window);
			glfwTerminate();
		}


		const GLFWwindow * GetWindow() const { return m_window; }
		GLFWwindow * GetWindow() { return m_window; }
		VkExtent2D GetFramebufferSize() {
			int width; int height;
			glfwGetFramebufferSize(m_window, &width, &height);
			return { static_cast<uint32_t>(width) ,static_cast<uint32_t>(height) };
		}
		VkSurfaceKHR GetSurface() { return m_surface; }

	private:
		GLFWwindow * m_window;
		VkInstance m_vkInstance;
		VkSurfaceKHR m_surface;
	};
}