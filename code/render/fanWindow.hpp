#pragma once

#include "glfw/glfw3.h"
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()

#include "render/core/fanSwapChain.hpp"
#include "render/core/fanInstance.hpp"
#include "render/core/fanDevice.hpp"
#include "scene/singletons/fanMouse.hpp"

namespace fan
{
	//========================================================================================================
	// Abstraction of the glfw window
	//========================================================================================================
	class Window
	{
	public:
        struct InputData
        {
            GLFWwindow * mWindow = nullptr;
            Mouse mMouse;
        };

		void Create( const char* _name, const glm::ivec2 _position, const glm::ivec2 _size );
        void CreateGLFWWIndow(const char* _name, const glm::ivec2 _position, const glm::ivec2 _size);
        void PostCreateWindow(const glm::ivec2 _size);
        void SetFullscreen();
        void SetWindowed(  const glm::ivec2 _position, const glm::ivec2 _size );

		void Destroy();
        void DestroyWindow();

        VkExtent2D	GetExtent()	 const;
        glm::ivec2	GetSize()	 const;
		glm::ivec2	GetPosition() const;
		bool		IsOpen() const;
        bool        IsFullscreen() const;

        static void MakeValidPositionAndSize( glm::ivec2& _position,  glm::ivec2& _size );
        static InputData& GetInputData( GLFWwindow* _window );

        using GetWindowUserPtrFunc = void* ( * )( GLFWwindow* _window );
        static GetWindowUserPtrFunc sGetWindowUserPtr;
		GLFWwindow*		mWindow = nullptr;
		Instance		mInstance;
		Device			mDevice;
		SwapChain		mSwapchain;
		VkSurfaceKHR    mSurface;
        InputData       mInputData;

	};
}