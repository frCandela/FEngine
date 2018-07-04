#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif // !GLFW_INCLUDE_VULKAN

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#include <vector>

//Contains the properties of a swap chain for device compatibility
struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

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

const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
const std::vector<const char*> validationLayers = { "VK_LAYER_LUNARG_standard_validation" };

class Device
{
public:
	Device(VkInstance& instance) : m_instance(instance){}

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
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
};
