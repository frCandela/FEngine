#pragma once

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <vector>
#include <set>


#include "Window.h"
#include "vulkan/Vertex.h"
#include "vulkan/SwapChain.h"
#include "vulkan/Sampler.h"
#include "vulkan/Instance.h"
#include "vulkan/Buffer.h"
#include "vulkan/Shader.h"
#include "vulkan/RenderPass.h"
#include "vulkan/Texture.h"
#include "vulkan/DepthImage.h"	
#include "vulkan/Descriptors.h"
#include "vulkan/Commands.h"


#include "Camera.h"


class Renderer
{
public:
	Renderer(Window& rWindow, Camera& rCamera);
	~Renderer();

	Camera* m_pCamera;

	const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

	Window& m_window;

	vk::Texture* texture; 
	vk::Sampler* textureSampler;

	std::vector<vk::Buffer*> buffers;

	vk::Descriptors* descriptors;

	vk::Commands* commands;
	vk::Instance* instance;
	vk::Device* device;
	vk::SwapChain* swapChain;
	vk::RenderPass * renderPass;

	vk::Shader* vertShader;
	vk::Shader* fragShader;

	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	std::vector<VkSemaphore> imageAvailableSemaphores;//Specifies that an image has been acquired and is ready for rendering
	std::vector<VkSemaphore> renderFinishedSemaphores;//Specifies that rendering has finished and presentation can happen
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;

	void createCommandBuffers();
	void recreateSwapChain();
	void createGraphicsPipeline();
	void createSyncObjects();
	void cleanup();

	void drawFrame();
};

