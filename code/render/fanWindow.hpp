#pragma once

#include "glfw/glfw3.h"
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()

#include "render/core/fanSwapChain.hpp"
#include "render/core/fanInstance.hpp"
#include "render/core/fanDevice.hpp"

namespace fan
{

	//================================================================================================================================
	// Abstraction of the glfw window
	//================================================================================================================================
	class Window
	{
	public:
		void Create( const char* _name, const glm::ivec2 _size, const glm::ivec2 _position );
		void Destroy();

		VkExtent2D	GetExtent()	 const;
		glm::ivec2	GetPosition() const;
		bool		IsOpen() const;

		GLFWwindow*		mWindow = nullptr;
		Instance		mInstance;
		Device			mDevice;
		SwapChain		mSwapchain;
		VkSurfaceKHR	mSurface;
	};
}