#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif // !GLFW_INCLUDE_VULKAN

#include <vector>

namespace vk
{
	class Device;
	class DepthImage;
	class Image;
}

#include "DepthImage.h"
#include "Device.h"

namespace vk
{

	class SwapChain
	{
	public:
		SwapChain(Device& device);
		~SwapChain();


		DepthImage* depthImage;//Depth

		/// Cleans up the old versions of the swap chain objects 
		void CleanupSwapChain();

		/// Create the swapChain, the image view and the depth buffer
		void BuildSwapChain(Window& window);

		/// Creates a frameBuffer for each swapChain image view
		void CreateFramebuffers(VkRenderPass& renderPass);


		//Zob
		std::vector<VkFramebuffer> swapChainFramebuffers;
		VkExtent2D swapChainExtent;
		VkSwapchainKHR swapChain;
		VkFormat swapChainImageFormat;

	private:
		Device & m_device;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;

		/// Creates the best swap chain possible depending on the device capabilities.
		void CreateSwapChain(Window& window);

		/// Creates the images views of the swap chain. An image view describes how to access the image and which part of the image to access (2D texture, depth texture, mipmapping levels etc.)
		void CreateImageViews();

		/// Choose the best SurfaceFormat in a list of available formats (color space)
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		/// Choose the best presentation mode in a list of available presentation mode
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);

		/// Returns the best available swap extent of a surface (resolution of the swap chain images)
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, Window& window);
	};
}



