#pragma once

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <vector>
#include <set>


#include "Window.h"
#include "Vertex.h"
#include "SwapChain.h"
#include "Sampler.h"
#include "Instance.h"
#include "Buffer.h"
#include "Shader.h"
#include "RenderPass.h"
#include "Texture.h"
#include "DepthImage.h"	
#include "Descriptors.h"
#include "Commands.h"


class Renderer
{
public:
	Renderer(Window& window);
	~Renderer();

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

