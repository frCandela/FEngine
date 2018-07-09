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
#include <chrono>

#include "Vertex.h"
#include "SwapChain.h"
#include "Sampler.h"
#include "Instance.h"
#include "Buffer.h"
#include "Shader.h"
#include "RenderPass.h"

class FEngine
{
public:
	FEngine();

	void Run();

	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
	GLFWwindow * window;

	VkBuffer uniformBuffer;
	VkDeviceMemory uniformBufferMemory;

	Image* textureImage; 


	void zobCleanup();


	Buffer* buffer;

	Shader vertShader;
	Shader fragShader;

	struct UniformBufferObject 
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};
	static Instance* instance;

	Sampler* textureSampler;
	static Device* device;
	static VkCommandPool commandPool;
	static SwapChain* swapChain;

	RenderPass * renderPass;
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

	void initWindow();
	void initVulkan();
	void recreateSwapChain();

	void createDescriptorSetLayout();
	void createGraphicsPipeline();

	void createDescriptorPool();
	void createDescriptorSet();

	void createCommandPool();

	void createCommandBuffers();
	void drawFrame();
	void createSyncObjects();

	//Buffers creation
	void createUniformBuffer();
	void updateUniformBuffer();
	void mainLoop();
	void cleanup();
};

