#pragma once


#include "AllInclude.h"

#include "vkInstance.h"
#include "vkDevice.h"
#include "vkWindow.h"
#include "vkSwapChain.h"

class Renderer {
public:
	void Run() 
	{
		VkExtent2D size{ 800, 600 };
		Instance instance;
		Window window("Vulkan", size, instance.vkInstance );
		Device device(instance, window.GetSurface() );
		SwapChain swapchain(device, window.GetSurface(), size);

		while ( ! glfwWindowShouldClose(window.GetWindow()))
		{
			glfwPollEvents();
		}
	}
};