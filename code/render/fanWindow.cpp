#include "render/fanWindow.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"
#include "core/input/fanInput.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void Window::Create( const char* _name, const glm::ivec2 _size, const glm::ivec2 _position )
	{
		mInstance.Create();

		glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
		glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );
		mWindow = glfwCreateWindow( _size.x, _size.y, _name, nullptr/* fullscreen monitor */, nullptr );
		glfwCreateWindowSurface( mInstance.mInstance, mWindow, nullptr, &mSurface );
		Debug::Log() << std::hex << "VkSurfaceKHR          " << mSurface << std::dec << Debug::Endl();

		glfwSetWindowPos( mWindow, _position.x, _position.y );

		mDevice.Create( mInstance, mSurface );
		mSwapchain.Create( mDevice, mSurface, { (uint32_t)_size.x, (uint32_t)_size.y } );

		Input::Get().Setup( mWindow );
	}

	//======================================================c==========================================================================
	//================================================================================================================================
	void Window::Destroy()
	{
		mSwapchain.Destroy( mDevice );
		mDevice.Destroy();

		vkDestroySurfaceKHR( mInstance.mInstance, mSurface, nullptr );
		mSurface = VK_NULL_HANDLE;
		glfwDestroyWindow( mWindow );
		glfwTerminate();

		mInstance.Destroy();
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Window::IsOpen() const
	{
		return !glfwWindowShouldClose( mWindow );
	}

	//================================================================================================================================
	//================================================================================================================================
	VkExtent2D Window::GetExtent() const
	{
		int width; int height;
		glfwGetFramebufferSize( mWindow, &width, &height );
		return { static_cast< uint32_t >( width ) ,static_cast< uint32_t >( height ) };
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::ivec2	Window::GetPosition() const
	{
		glm::ivec2 position;
		glfwGetWindowPos( mWindow, &position.x, &position.y );
		return position;
	}
}
