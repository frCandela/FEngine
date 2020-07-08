#pragma once

#include "glfw/glfw3.h"
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()

#include "render/core/fanSwapChain.hpp"

namespace fan
{
	class Device;
	class Instance;

	//================================================================================================================================
	// Abstraction of the glfw window
	//================================================================================================================================
	class Window
	{
	public:
		Window( const char* _name, const glm::ivec2 _size, const glm::ivec2 _position );
		~Window();

		GLFWwindow*			GetWindow()			  { return m_window; }
		const GLFWwindow*	GetWindow()		const { return m_window; }
		SwapChain&			GetSwapChain() { return m_swapchain; }
		Instance*			GetInstance()	const { return m_instance; }
		Device&				GetDevice()		const { return *m_device; }
		VkSurfaceKHR		GetSurface()	const { return m_surface; }
		VkExtent2D			GetExtent()		const;
		glm::ivec2			GetPosition()	const;

		bool IsOpen() const;

	private:
		GLFWwindow*		m_window = nullptr;
		Instance*		m_instance;
		Device*			m_device = nullptr;
		SwapChain		m_swapchain;
		VkSurfaceKHR	m_surface;
	};
}