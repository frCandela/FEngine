#include "Includes.h"

#include "vulkan/vkWindow.h"

namespace vk {

	//================================================================================================================================
	//================================================================================================================================
	Window::Window(const char * _name, VkExtent2D size, VkInstance _vkInstance) :
		m_vkInstance(_vkInstance) {
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		m_window = glfwCreateWindow(size.width, size.height, _name, nullptr/* fullscreen monitor */, nullptr);
		glfwCreateWindowSurface(_vkInstance, m_window, nullptr, &m_surface);
		std::cout << std::hex << "VkSurfaceKHR\t\t" << m_surface << std::dec << std::endl;
	}

	//================================================================================================================================
	//================================================================================================================================
	Window::~Window() {
		vkDestroySurfaceKHR(m_vkInstance, m_surface, nullptr);
		m_surface = VK_NULL_HANDLE;
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	//================================================================================================================================
	//================================================================================================================================
	VkExtent2D Window::GetExtent() {
		int width; int height;
		glfwGetFramebufferSize(m_window, &width, &height);
		return { static_cast<uint32_t>(width) ,static_cast<uint32_t>(height) };
	}
}