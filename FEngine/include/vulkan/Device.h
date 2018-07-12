#pragma once


#ifndef GLM_FORCE_RADIANS
	#define GLM_FORCE_RADIANS
	#define GLM_FORCE_DEPTH_ZERO_TO_ONE
	#include "glm/glm.hpp"
	#include "glm/gtc/matrix_transform.hpp"
#endif // !GLM_FORCE_RADIANS

#include <vector>

#include "Window.h"
#include "Commands.h"

namespace vk
{
	class Commands;

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
	};

	//Contains the properties of a swap chain for device compatibility
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	const std::vector<const char*> validationLayers = { "VK_LAYER_LUNARG_standard_validation" };

	class Device
	{
	public:
		Device(VkInstance& instance, Window& window);

		~Device()
		{
			vkDestroyDevice(device, nullptr);
		}

		Commands * commands;
		SwapChainSupportDetails swapChainSupportDetails;
		VkInstance& m_instance;
		VkDevice device;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkSurfaceKHR surface;

		void pickPhysicalDevice();
		void createLogicalDevice();
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		bool isDeviceSuitable(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR& surface);

	};

}