#include "renderer/GLFWWindow.h"

Window::Window(uint32_t width, uint32_t height, std::string name) :
	m_width(width),
	m_height(height)
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);// No opengl context
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	m_window = glfwCreateWindow(width, height, name.c_str(), nullptr/* fullscreen monitor */, nullptr);

	Input::Setup(m_window);


}

bool Window::WindowOpen() const
{
	return !glfwWindowShouldClose(m_window);
}

int Window::GetRefreshRate()
{
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
	return videoMode->refreshRate;
}

void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
	if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS)
		throw std::runtime_error("failed to create window surface!");
}

VkExtent2D Window::GetExtend2D() const
{
	int width, height;
	glfwGetFramebufferSize(m_window, &width, &height);

	return VkExtent2D {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
	};
}

Window::~Window()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}