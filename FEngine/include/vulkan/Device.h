#pragma once


#ifndef GLM_FORCE_RADIANS
	#define GLM_FORCE_RADIANS
	#define GLM_FORCE_DEPTH_ZERO_TO_ONE
	#include "glm/glm.hpp"
	#include "glm/gtc/matrix_transform.hpp"
#endif // !GLM_FORCE_RADIANS

#include <vector>
#include "renderer/GLFWWindow.h"

namespace vk
{
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	//const std::vector<const char*> validationLayers = { "VK_LAYER_LUNARG_standard_validation" };
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_GOOGLE_threading",
		//"VK_LAYER_LUNARG_parameter_validation",
		"VK_LAYER_LUNARG_object_tracker",
		"VK_LAYER_LUNARG_core_validation"
		//"VK_LAYER_LUNARG_swapchain",
		//"VK_LAYER_GOOGLE_unique_objects"
	};

	class Device
	{
	public:

		Device(VkInstance instance, Window& window);
		~Device();
		
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;		
		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkQueue presentQueue = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;

		VkPhysicalDeviceProperties properties;

		//Contains the vulkan queues families used
		struct QueueFamilyIndices
		{
			int graphicsFamily = -1;	//queue family for drawing commands
			int presentFamily = -1;		//queue family to present images to the surface

										//Returns true if all families are represented
			bool isComplete()
			{
				return graphicsFamily >= 0 && presentFamily >= 0;
			}
		} queueFamilyIndices;

		//Contains the properties of a swap chain for device compatibility
		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		} swapChainSupportDetails;
	
		
		// Find the right type of memory 
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		// Returns the swap chain details of a physical device (surface formats and presentation modes)
		void QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR& surface);

	private:

		// Look for and select a graphics card in the system
		void PickPhysicalDevice(VkInstance instance);

		// Creates the logical device (interface with the physical device)
		void CreateLogicalDevice();

		// Check if the required extensions are supported on the GPU
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		
		// Returns true if the physical device is suitable for the application
		bool IsDeviceSuitable(VkPhysicalDevice device);		

		// Get the queue families needed for rendering 
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	};

}