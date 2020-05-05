#pragma once

#include "fanGLFW.hpp"
#include "fanGLM.hpp"

namespace fan
{
	class Device;
	class SwapChain;
	class Instance;

	//================================================================================================================================
	// Abstraction of the glfw window
	//================================================================================================================================
	class Window
	{
	public:
		Window( const char* _name, const VkExtent2D size, const glm::ivec2 _position );
		~Window();

		GLFWwindow*			GetWindow()			  { return m_window; }
		const GLFWwindow*	GetWindow()		const { return m_window; }
		Instance*			GetInstance()	const { return m_instance; }
		Device&				GetDevice()		const { return *m_device; }
		SwapChain&			GetSwapChain()	const { return *m_swapchain; }
		VkSurfaceKHR		GetSurface()	const { return m_surface; }
		VkExtent2D			GetExtent()		const;
		glm::ivec2			GetPosition()	const;

		bool IsOpen() const;

	private:
		GLFWwindow*		m_window = nullptr;
		Instance*		m_instance;
		Device*			m_device = nullptr;
		SwapChain*		m_swapchain;
		VkSurfaceKHR	m_surface;
	};
}