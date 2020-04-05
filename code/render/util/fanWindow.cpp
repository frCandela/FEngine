#include "render/util/fanWindow.hpp"

#include "render/core/fanSwapChain.hpp"
#include "render/core/fanInstance.hpp"
#include "render/core/fanDevice.hpp"
#include "core/input/fanInput.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Window::Window( const char* _name, const VkExtent2D _size, const glm::ivec2 _position )
	{

		m_instance = new Instance();

		glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
		glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );
		m_window = glfwCreateWindow( _size.width, _size.height, _name, nullptr/* fullscreen monitor */, nullptr );
		glfwCreateWindowSurface( m_instance->vkInstance, m_window, nullptr, &m_surface );
		Debug::Get() << Debug::Severity::log << std::hex << "VkSurfaceKHR          " << m_surface << std::dec << Debug::Endl();

		glfwSetWindowPos( m_window, _position.x, _position.y );

		m_device = new Device( m_instance, m_surface );
		m_swapchain = new SwapChain( *m_device, m_surface, _size );

		Input::Get().Setup( m_window );
	}

	//======================================================c==========================================================================
	//================================================================================================================================
	Window::~Window()
	{
		delete m_swapchain;
		delete m_device;

		vkDestroySurfaceKHR( m_instance->vkInstance, m_surface, nullptr );
		m_surface = VK_NULL_HANDLE;
		glfwDestroyWindow( m_window );
		glfwTerminate();

		delete m_instance;
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
