#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif // !GLFW_INCLUDE_VULKAN

#include <vector>

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
	Device(VkInstance& instance, VkCommandPool& commandPool) : 
		m_instance(instance),
		m_commandPool(commandPool)
	{}

	~Device()
	{
		vkDestroyDevice(device, nullptr);
	}


	VkInstance& m_instance;
	VkDevice device;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSurfaceKHR surface;

	VkCommandPool& m_commandPool;

	void pickPhysicalDevice();
	void createLogicalDevice();
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
};
