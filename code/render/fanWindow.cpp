#include "render/fanWindow.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"
#include "core/input/fanInput.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Window::Window( const char* _name, const glm::ivec2 _size, const glm::ivec2 _position )
	{
		m_instance.Create();

		glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
		glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );
		m_window = glfwCreateWindow( _size.x, _size.y, _name, nullptr/* fullscreen monitor */, nullptr );
		glfwCreateWindowSurface( m_instance.instance, m_window, nullptr, &m_surface );
		Debug::Log() << std::hex << "VkSurfaceKHR          " << m_surface << std::dec << Debug::Endl();

		glfwSetWindowPos( m_window, _position.x, _position.y );

		m_device.Create( m_instance, m_surface );
		m_swapchain.Create( m_device, m_surface, { (uint32_t)_size.x, (uint32_t)_size.y } );

		Input::Get().Setup( m_window );
	}

	//======================================================c==========================================================================
	//================================================================================================================================
	Window::~Window()
	{
		m_swapchain.Destroy( m_device );
		m_device.Destroy();

		vkDestroySurfaceKHR( m_instance.instance, m_surface, nullptr );
		m_surface = VK_NULL_HANDLE;
		glfwDestroyWindow( m_window );
		glfwTerminate();

		m_instance.Destroy();
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Window::IsOpen() const
	{
		return !glfwWindowShouldClose( m_window );
	}

	//================================================================================================================================
	//================================================================================================================================
	VkExtent2D Window::GetExtent() const
	{
		int width; int height;
		glfwGetFramebufferSize( m_window, &width, &height );
		return { static_cast< uint32_t >( width ) ,static_cast< uint32_t >( height ) };
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::ivec2	Window::GetPosition() const
	{
		glm::ivec2 position;
		glfwGetWindowPos( m_window, &position.x, &position.y );
		return position;
	}
}
