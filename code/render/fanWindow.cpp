#include "render/fanWindow.hpp"

#include "core/fanDebug.hpp"
#include "core/input/fanInput.hpp"

namespace fan
{
    Window::GetWindowUserPtrFunc Window::sGetWindowUserPtr = &glfwGetWindowUserPointer;

	//========================================================================================================
	//========================================================================================================
	void Window::Create( const char* _name, const glm::ivec2 _position, const glm::ivec2 _size )
	{
        mInstance.Create( &mDevice.mDebugNames );

        glm::ivec2 validSize = _size;
        glm::ivec2 validPosition = _position;
        MakeValidPositionAndSize( validPosition, validSize );

        CreateGLFWWIndow( _name, validPosition, validSize );

		mDevice.Create( mInstance, mSurface );

		PostCreateWindow(validSize);
	}

    //========================================================================================================
    //========================================================================================================
    void Window::PostCreateWindow(const glm::ivec2 _size )
    {
        mSwapchain.Create( mDevice, mSurface, { (uint32_t)_size.x, (uint32_t)_size.y } );

        Input::Get().Setup( mWindow );
        mInputData.mMouse.Clear();
        glfwSetWindowUserPointer( mWindow, &mInputData );
        mInputData.mWindow = mWindow;
        fanAssert(glfwGetWindowUserPointer( mWindow ) == &mInputData) ;
    }

    //========================================================================================================
    //========================================================================================================
    void Window::MakeValidPositionAndSize( glm::ivec2& _position,  glm::ivec2& _size )
    {
        if( _size.x <= 0 || _size.y <= 0)
        {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            int x, y, w, h;
            glfwGetMonitorWorkarea( monitor, &x, &y, &w, &h );
            _size     = glm::ivec2( w, h - 30 );
            _position = glm::ivec2( x, y + 30 );
        }
    }

    //========================================================================================================
    //========================================================================================================
    void Window::SetFullscreen()
    {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        int x, y, w, h;
        glfwGetMonitorWorkarea( monitor, &x, &y, &w, &h );
        glfwSetWindowMonitor( mWindow, monitor, x, y, w, h, GLFW_DONT_CARE );
    }

    //========================================================================================================
    //========================================================================================================
    void Window::SetWindowed( const glm::ivec2 _position, const glm::ivec2 _size )
    {
	    glm::ivec2 pos = _position, size = _size;
	    MakeValidPositionAndSize( pos, size );
        glfwSetWindowMonitor( mWindow, nullptr, pos.x, pos.y, size.x, size.y, GLFW_DONT_CARE );
    }

    //========================================================================================================
    //========================================================================================================
    void Window::CreateGLFWWIndow(const char* _name, const glm::ivec2 _position, const glm::ivec2 _size )
    {
        glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
        glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );
        mWindow = glfwCreateWindow( _size.x, _size.y, _name, nullptr/* fullscreen monitor */, nullptr );
        glfwCreateWindowSurface( mInstance.mInstance, mWindow, nullptr, &mSurface );
        Debug::Log() << std::hex << "VkSurfaceKHR          " << mSurface << std::dec << Debug::Endl();
        glfwSetWindowPos( mWindow, _position.x, _position.y );


    }

	//========================================================================================================
	//========================================================================================================
	void Window::Destroy()
	{
		mSwapchain.Destroy( mDevice );
		mDevice.Destroy();
        DestroyWindow();
		mInstance.Destroy();
        glfwTerminate();
	}

    //========================================================================================================
    //========================================================================================================
    void Window::DestroyWindow()
	{
        vkDestroySurfaceKHR( mInstance.mInstance, mSurface, nullptr );
        mSurface = VK_NULL_HANDLE;
        glfwDestroyWindow( mWindow );

	}


    //========================================================================================================
    //========================================================================================================
    Window::InputData& Window::GetInputData( GLFWwindow* _window )
    {
        fanAssert( _window != nullptr );
        void* data = ( *Window::sGetWindowUserPtr )( _window );
        fanAssert( data != nullptr );
        return *static_cast<InputData*>(data);
    }

	//========================================================================================================
	//========================================================================================================
	bool Window::IsOpen() const
	{
		return !glfwWindowShouldClose( mWindow );
	}

    //========================================================================================================
    //========================================================================================================
    bool Window::IsFullscreen() const
    {
	    return glfwGetWindowMonitor( mWindow ) != nullptr;
    }

	//========================================================================================================
	//========================================================================================================
	VkExtent2D Window::GetExtent() const
	{
		int width; int height;
		glfwGetFramebufferSize( mWindow, &width, &height );
		return { static_cast< uint32_t >( width ) ,static_cast< uint32_t >( height ) };
	}

    //========================================================================================================
    //========================================================================================================
    glm::ivec2	Window::GetSize()	 const
    {
	    const VkExtent2D extent = GetExtent();
	    return glm::ivec2( extent.width, extent.height );
    }

	//========================================================================================================
	//========================================================================================================
	glm::ivec2	Window::GetPosition() const
	{
		glm::ivec2 position;
		glfwGetWindowPos( mWindow, &position.x, &position.y );
		return position;
	}
}
