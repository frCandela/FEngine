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
#include "vulkan/Mesh.h"
#include "vulkan/Shader.h"
#include "vulkan/RenderPass.h"
#include "vulkan/Texture.h"
#include "vulkan/DepthImage.h"	
#include "vulkan/Descriptors.h"
#include "vulkan/CommandPool.h"
#include "vulkan/CommandBuffer.h"

#include "ImguiManager.h"
#include "Camera.h"

// Main class for rendering objects using Vulkan
class Renderer
{
public:
	Renderer(Window& rWindow, Camera& rCamera);
	~Renderer();

	// Draw a frame
	void DrawFrame();

	// Returns the size of the swapChain extent
	glm::vec2 GetSize() const; 

private:

	// Setup the command buffers for drawing operations
	void CreateCommandBuffers();

	// Recreates the swap chain (necessary when the window is resized)
	void RecreateSwapChain();

	// Creates the graphics pipeline
	void CreateGraphicsPipeline();

	// Creates the sync objects (fences and semaphores)
	void CreateSyncObjects();

	// Creates meshes for test rendering
	void CreateTestMesh();

	// Clean Vulkan objects 
	void Cleanup();

	const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
	size_t currentFrame = 0;

	Camera* m_pCamera;
	Window& m_window;

	ImguiManager * imGui = nullptr;

	vk::Texture* texture;
	vk::Sampler* textureSampler;
	vk::Descriptors* descriptors;
	vk::CommandPool* commandPool;
	vk::CommandBuffer* commandBuffers;
	vk::Instance* instance;
	vk::Device* device;
	vk::SwapChain* swapChain;
	vk::RenderPass * renderPass;
	vk::Shader* vertShader;
	vk::Shader* fragShader;

	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	std::vector<vk::Mesh*> buffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;//Specifies that an image has been acquired and is ready for rendering
	std::vector<VkSemaphore> renderFinishedSemaphores;//Specifies that rendering has finished and presentation can happen
	std::vector<VkFence> inFlightFences;


};

