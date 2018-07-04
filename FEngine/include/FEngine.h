#pragma once

#ifndef GLFW_INCLUDE_VULKAN
	#define GLFW_INCLUDE_VULKAN
	#include <GLFW/glfw3.h>
#endif // !GLFW_INCLUDE_VULKAN


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <vector>
#include <set>

#include <fstream>
#include <chrono>

#include "Vertex.h"
#include "Image.h"
#include "Device.h"

class Image;

class FEngine
{
public:
	FEngine();

	void Run();

	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	const std::string MODEL_PATH = "models/chalet.obj";
	const std::string TEXTURE_PATH = "textures/chalet.jpg";

	const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
	GLFWwindow * window;

	//Buffers
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	VkBuffer uniformBuffer;
	VkDeviceMemory uniformBufferMemory;

	Image* textureImage; 
	VkSampler textureSampler;

	//Depth
	Image* depthImage;

	//Vertices and indices
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	struct UniformBufferObject 
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	void loadModel();


	
	static VkInstance instance;
	VkDebugReportCallbackEXT callback;

	static Device* device;


	static VkCommandPool commandPool;


	

	
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



	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;//Specifies that an image has been acquired and is ready for rendering
	std::vector<VkSemaphore> renderFinishedSemaphores;//Specifies that rendering has finished and presentation can happen
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;





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
	void createDepthResources();
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();
	static bool hasStencilComponent(VkFormat format) 
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	void createTextureImageView();
	void createTextureSampler();

	void createCommandBuffers();
	void drawFrame();
	void createSyncObjects();
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	//Buffers creation
	void createVertexBuffer();
	void createIndexBuffer();
	void createUniformBuffer();

	static VkCommandBuffer beginSingleTimeCommands();
	static void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	VkShaderModule createShaderModule(const std::vector<char>& code);
	void setupDebugCallback();
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
	static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	static std::vector<char> readFile(const std::string& filename);

	//swap chain capabilities selection
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);//resolution of the swap chain images
	
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(device->device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) 
			throw std::runtime_error("failed to create texture image view!");
		

		return imageView;
	}

	void updateUniformBuffer();
	void mainLoop();
	void cleanup();
};

