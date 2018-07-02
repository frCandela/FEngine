#pragma once


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <vector>
#include <set>
#include <algorithm>
#include <fstream>
#include <chrono>

#include "Vertex.h"




class FEngine
{
public:
	void Run();

	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;
	const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
	GLFWwindow * window;

	//Buffers
	VkBuffer vertexBuffer; 
	VkDeviceMemory vertexBufferMemory;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	VkBuffer uniformBuffer;
	VkDeviceMemory uniformBufferMemory;

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;

	const std::vector<Vertex> vertices = 
	{
		{ { -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f } },
		{ { 0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f } },
		{ { 0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f } },
		{ { -0.5f, 0.5f },{ 1.0f, 1.0f, 1.0f } }
	};

	const std::vector<uint16_t> indices = 
	{
		0, 1, 2, 2, 3, 0
	};


	struct UniformBufferObject 
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

private:

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
	
	VkInstance instance;
	VkDebugReportCallbackEXT callback;
	VkDevice device;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue;
	VkSurfaceKHR surface;
	VkQueue presentQueue;

	
	VkSwapchainKHR swapChain;//collection of buffers used for displaying frames
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;

	VkCommandPool commandPool;

	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;//Specifies that an image has been acquired and is ready for rendering
	std::vector<VkSemaphore> renderFinishedSemaphores;//Specifies that rendering has finished and presentation can happen
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;

	const std::vector<const char*> validationLayers = { "VK_LAYER_LUNARG_standard_validation" };
	const std::vector<const char*> deviceExtensions = {	VK_KHR_SWAPCHAIN_EXTENSION_NAME	};
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	//Vulkan callbacks
	static VkResult CreateDebugReportCallback(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback );
	static void DestroyDebugReportCallback(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData
	);

	

	void initWindow();
	void initVulkan();
	void createLogicalDevice();
	void recreateSwapChain();
	void cleanupSwapChain();
	void createSwapChain();
	void createImageViews();
	void createInstance();
	void createSurface();
	void createRenderPass();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void createFramebuffers();

	void createDescriptorPool();
	void createDescriptorSet();

	void createCommandPool();

	void createTextureImage();


	void createCommandBuffers();
	void drawFrame();
	void createSyncObjects();
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	//Buffers creation
	void createVertexBuffer();
	void createIndexBuffer();
	void createUniformBuffer();

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	VkShaderModule createShaderModule(const std::vector<char>& code);
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	void setupDebugCallback();
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	static std::vector<char> readFile(const std::string& filename);

	//swap chain capabilities selection
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);//resolution of the swap chain images
	
	void updateUniformBuffer();
	void mainLoop();
	void cleanup();
};

