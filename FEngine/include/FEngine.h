#pragma once

#ifndef GLFW_INCLUDE_VULKAN
	#define GLFW_INCLUDE_VULKAN
	#include <GLFW/glfw3.h>
#endif // !GLFW_INCLUDE_VULKAN

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <vector>
#include <set>

#include "Vertex.h"
#include "SwapChain.h"
#include "Sampler.h"
#include "Instance.h"
#include "Buffer.h"
#include "Shader.h"
#include "RenderPass.h"
#include "Descriptors.h"
#include "Commands.h"



class FEngine
{
public:
	FEngine();

	void Run();

	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
	GLFWwindow * window;

	Image* textureImage; 
	Sampler* textureSampler;

	void zobCleanup();

	Buffer* buffer;
	Descriptors* descriptors;
	Commands* commands;

	Shader vertShader;
	Shader fragShader;

	static Instance* instance;
	
	static Device* device;

	static SwapChain* swapChain;

	RenderPass * renderPass;

	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	std::vector<VkSemaphore> imageAvailableSemaphores;//Specifies that an image has been acquired and is ready for rendering
	std::vector<VkSemaphore> renderFinishedSemaphores;//Specifies that rendering has finished and presentation can happen
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;

	void initWindow();
	void initVulkan();
	void recreateSwapChain();

	void createGraphicsPipeline();



	void drawFrame();

	void createSyncObjects();

	void mainLoop();
	void cleanup();
};

